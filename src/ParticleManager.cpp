#include "ParticleManager.hpp"
#include "Particle.hpp"
#include <functional>
#include <glm/geometric.hpp>
#include <strings.h>
#include <iostream>

ParticleManager::ParticleManager()
{

}

void ParticleManager::addParticle(Particle particle)
{
    
    particleList_.push_back(particle);
}

void ParticleManager::step()
{
    for(int i=0; i<particleList_.size(); ++i)
    {
        Particle p = particleList_[i];
        // apply velocity to position
        p.pos += p.v;

        p.v.y -= 0.001f;

        // apply drag to velocity
        p.v += -dragStrength * glm::length(p.v)*p.v;

        for(int j=0; j<particleList_.size(); ++j)
        {
            if(i==j) continue ;
            Particle collisionP = particleList_[j];

            if(collision(p, collisionP))
            {
                // point of incidence
                glm::vec3 colPoint = collisionPoint(p, collisionP);
                glm::vec3 collisionNormal = glm::normalize(p.pos-collisionP.pos);
                p.v = reflectionRay(p.v, collisionNormal, 0.9);

            }

        }

        // ground plane collision
        if(p.pos.y+p.rad<=0)
            p.v = reflectionRay(p.v, glm::vec3(0.0f, 1.0f, 0.0f), 0.9);


        particleList_[i] = p;
    }
}

glm::vec3 ParticleManager::reflectionRay(const glm::vec3& velocity, const glm::vec3& normal, float elasticity)
{
    glm::vec3 vNormal = glm::dot(velocity, normal) * normal;
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

