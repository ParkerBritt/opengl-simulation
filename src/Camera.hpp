#pragma once
#include <glm/fwd.hpp>
#include <glm/glm.hpp>

class Camera
{
public:
    Camera();
    Camera(float posX, float posY, float posZ);
    glm::mat4 getViewMatrix();
    void setPos(float x, float y, float z);
    void movePos(float x, float y, float z);
    void rotateAroundCenter(float angle, glm::vec3 axis);
    void changeRadius(float delta);
    void changeCenter(float x, float y, float z);
    void setCenter(float x, float y, float z);

    glm::vec3 getForward();
    glm::vec3 getRight();
    glm::vec3 getUp();


private:
    glm::mat4 viewMatrix_{1.0f};
    glm::vec3 camPos_{0.0f,0.0f,0.0f};
    glm::vec3 camCenter_{0.0f,0.0f,0.0f};
    glm::vec3 camUp_{0.0f,1.0f,0.0f};
};
