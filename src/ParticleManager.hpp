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
};
