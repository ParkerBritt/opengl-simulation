#include "ParticleManager.hpp"
#include "Particle.hpp"
#include <exception>
#include <functional>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <stdexcept>
#include <string>
#include <strings.h>
#include <iostream>
#include <algorithm>
#include "MathUtils.h"

ParticleManager::ParticleManager()
{
    initPartitions();
}

void ParticleManager::initPartitions()
{
    boundsLower = glm::vec3(-50, 0, -50);
    boundsUpper = glm::vec3(50, 100, 50);


    partitionSizeX_ = std::abs(boundsUpper.x-boundsLower.x)/static_cast<float>(numPartitionsX_);
    partitionSizeY_ = std::abs(boundsUpper.y-boundsLower.y)/static_cast<float>(numPartitionsY_);
    partitionSizeZ_ = std::abs(boundsUpper.z-boundsLower.z)/static_cast<float>(numPartitionsZ_);
    
    std::cout << "partitionSizeX_: " << partitionSizeX_ << "\n";
    std::cout << "partitionSizeY_: " << partitionSizeY_ << "\n";
    std::cout << "partitionSizeZ_: " << partitionSizeZ_ << "\n";


    partitions_ = std::vector<std::unordered_set<size_t>>(numPartitionsX_*numPartitionsY_*numPartitionsZ_);


}

void ParticleManager::addParticle(Particle particle)
{
    particle.id = maxId_++;
    // particle.partionIndex = getPartition(particle.pos);
    particleList_.push_back(particle);
}


size_t ParticleManager::convertPartitionIndex(size_t x, size_t y, size_t z)
{
    return
    (
        x + numPartitionsX_ * (y + numPartitionsY_ * z)
    );
}

size_t ParticleManager::getPartitionComponent(float position, float boundsLower, float boundsUpper, size_t axisDivisions)
{
    size_t indx;
    position = std::clamp(position, boundsLower, boundsUpper);
    float normalizePos = remap<float>(position, boundsLower, boundsUpper, 0, 1);
    indx = normalizePos * (axisDivisions-1);
    return indx;
}

size_t ParticleManager::getPartition(glm::vec3 position)
{
    return  convertPartitionIndex(
        getPartitionComponent(position.x, boundsLower.x, boundsUpper.x, numPartitionsX_),
        getPartitionComponent(position.y, boundsLower.y, boundsUpper.y, numPartitionsY_),
        getPartitionComponent(position.z, boundsLower.z, boundsUpper.z, numPartitionsZ_)
    );
}

void ParticleManager::step(double dt)
{
    for(int i=0; i<particleList_.size(); ++i)
    {
        Particle& p = particleList_[i];
        // apply velocity to position
        p.pos += p.v * glm::vec3(dt);

        // safety check
        if(p.partionIndex>=partitions_.size())
            throw std::out_of_range("partition " + std::to_string(p.partionIndex) + " out of range before move, max:" + std::to_string(partitions_.size()));

        // remove from old partition
        partitions_[p.partionIndex].erase(p.id);
        // get new partition
        p.partionIndex = getPartition(p.pos);

        // safety check
        if(p.partionIndex>=partitions_.size())
        {
            int debugIndx = i;
            std::cout << "------\nindex:\t" << particleList_[debugIndx].partionIndex << "\n";
            std::cout << "pos:\t" << particleList_[debugIndx].pos.x << " " << particleList_[debugIndx].pos.y << " " << particleList_[debugIndx].pos.z << " " << "\n";
            std::cout << "prt Comp:\t" <<   
                getPartitionComponent(particleList_[debugIndx].pos.x, boundsLower.x, boundsUpper.x, numPartitionsX_) << " " <<
                getPartitionComponent(particleList_[debugIndx].pos.y, boundsLower.y, boundsUpper.y, numPartitionsY_) << " " <<
                getPartitionComponent(particleList_[debugIndx].pos.z, boundsLower.z, boundsUpper.z, numPartitionsZ_) << "\n";
            throw std::out_of_range(
                "partition " + std::to_string(p.partionIndex) + " out of range after move, max:" + std::to_string(partitions_.size()) +
                "\nposition: " + std::to_string(p.pos.x) + " " + std::to_string(p.pos.y) + " " + std::to_string(p.pos.z) + "\n"
            );
        }

        // add to new partition
        partitions_[p.partionIndex].insert(p.id);
    }

    // debug prints
    // for(size_t i=0; i<partitions_.size(); ++i)
    // {
    //     auto partition = partitions_[i];
    //     std::cout << "partition: " << i << "\n";
    //     for(auto item : partition)
    //     {
    //         std::cout << item << "\n";
    //     }
    // }



    // std::vector<Particle> lockedParticles = particleList_;
    std::vector<Particle> lockedParticles = particleList_;

    for(int i=0; i<particleList_.size(); ++i)
    {
        float dragStrength = dragStrengthGlobal_;
        Particle& p = particleList_[i];
        // std::cout << "deltatime: " << dt << "\n";

        std::unordered_set<size_t>& partition = partitions_[p.partionIndex];

        // std::cout << "comparing against: " << partition.size() << "\n";
        for(auto it = partition.begin(); it!=partition.end(); ++it)
        {
            size_t otherParticleID = *it;
            if(i==otherParticleID) continue ;
            Particle collisionP = lockedParticles[otherParticleID];

            if(collision(p, collisionP))
            {
                // point of incidence
                glm::vec3 colPoint = collisionPoint(p, collisionP);
                glm::vec3 collisionNormal = glm::normalize(p.pos-collisionP.pos);

                // uncollide
                p.pos += collisionNormal * overlap(p, collisionP);

                glm::vec3 refRay = reflectionRay(p.v, collisionNormal, 0.2);
                p.v = refRay;

            }

        }

        // ground plane collision
        if(p.pos.y-p.rad<=0)
        {
            p.v = reflectionRay(p.v, glm::vec3(0.0f, 1.0f, 0.0f), 0.7);
            p.pos.y -= p.pos.y-p.rad;
            dragStrength+=0.9;
        }

        float gravity = 9.8f;
        p.v.y -= gravity*dt;

        // apply drag to velocity
        p.v += -dragStrength * glm::length(p.v)*p.v * glm::vec3(dt);

    }
}

glm::vec3 ParticleManager::reflectionRay(const glm::vec3& velocity, const glm::vec3& normal, float elasticity)
{
    // normal component of velocity
    glm::vec3 vNormal = glm::dot(velocity, normal) * normal;
    // tangent component of velocity
    glm::vec3 vTangent = velocity-vNormal;
    return vTangent-elasticity*vNormal;
}


glm::vec3 ParticleManager::collisionPoint(Particle p1, Particle p2)
{
    glm::vec3 vecToSurface = p1.rad*glm::normalize(p2.pos-p1.pos);
    return p1.pos+vecToSurface;
}


bool ParticleManager::collision(Particle p1, Particle p2)
{
    return overlap(p1, p2)>=0;
}

float ParticleManager::overlap(Particle p1, Particle p2)
{
    return p1.rad+p2.rad-glm::distance(p1.pos, p2.pos);
}

int ParticleManager::numParticles()
{
    return particleList_.size();
}

Particle ParticleManager::getParticle(int index)
{
    return particleList_[index]; 
}

