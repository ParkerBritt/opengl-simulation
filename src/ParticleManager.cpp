#include "ParticleManager.hpp"
#include "Particle.hpp"
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
        // ground plane collision
        if(p.y+p.rad<=0)
            continue;
        p.y -= 0.02f;
        particleList_[i] = p;
    }
}

int ParticleManager::numParticles()
{
    return particleList_.size();
}

Particle ParticleManager::getParticle(int index)
{
    return particleList_[index]; 
}

