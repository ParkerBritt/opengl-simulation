#pragma once
#include <glm/glm.hpp>

struct Particle
{
    glm::vec3 pos=glm::vec3(0.0f,0.0f,0.0f);
    glm::vec3 v=glm::vec3(0.0f,0.0f,0.0f);
    float rad=0.5;
    size_t id=0;
    size_t partionIndex=0;
};
