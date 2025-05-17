#pragma once

#include "Particle.hpp"
#include <vector>

class ParticleManager
{
public:
    ParticleManager();
    void addParticle(Particle particle);
    void step();
    int numParticles();
    Particle getParticle(int index);

private:
    std::vector<Particle> particleList_;
    bool collision(Particle p1, Particle p2);
    glm::vec3 collisionPoint(Particle p1, Particle p2);
    glm::vec3 reflectionRay(const glm::vec3& velocity, const glm::vec3& normal, float elasticity);
    float dragStrength = 0.95;
};
