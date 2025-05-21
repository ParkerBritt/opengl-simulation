#pragma once

#include "Particle.hpp"
#include <unordered_set>
#include <vector>

class ParticleManager
{
public:
    ParticleManager();
    void addParticle(Particle particle);
    void step(double deltaTime);
    int numParticles();
    Particle& getParticle(int index);

private:
    void initPartitions();
    std::vector<Particle> particleList_;
    bool collision(Particle p1, Particle p2);
    glm::vec3 collisionPoint(Particle p1, Particle p2);
    glm::vec3 reflectionRay(const glm::vec3& velocity, const glm::vec3& normal, float elasticity);
    float dragStrengthGlobal_ = 0.95;
    float overlap(Particle p1, Particle p2);
    glm::vec3 boundsUpper;
    glm::vec3 boundsLower;
    std::vector<std::unordered_set<size_t>> partitions_;
    size_t maxId_=0;

    size_t numPartitionsX_ = 100;
    size_t numPartitionsY_ = 100;
    size_t numPartitionsZ_ = 100;

    float partitionSizeX_;
    float partitionSizeY_;
    float partitionSizeZ_;

    size_t getPartition(glm::vec3 position);
    size_t getPartitionComponent(float position, float boundsLower, float boundsUpper, size_t axisDivisions);

    size_t convertPartitionIndex(size_t x, size_t y, size_t z);

};
