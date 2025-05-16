#include "Camera.hpp"
#include <glm/ext/quaternion_geometric.hpp>
#include <glm/fwd.hpp>
#include <glm/geometric.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <glm/ext/quaternion_trigonometric.hpp>
#include <glm/ext/quaternion_float.hpp>

void printMatrix(const glm::mat4& matrix) {
    for (int i = 0; i < 4; ++i) {
        for (int j = 0; j < 4; ++j) {
            std::cout << matrix[i][j] << " ";
            }
            std::cout << std::endl;
        }
}
void printVec(const glm::vec3& vector) {
    for (int i = 0; i < 3; ++i) {
        std::cout << vector[i] << " ";
    }
    std::cout << std::endl;
}

void Camera::changeCenter(float x, float y, float z)
{
    camCenter_.x += x;
    camCenter_.y += y;
    camCenter_.z += z;

    viewMatrix_ = glm::lookAt(
        camPos_,
        camCenter_,
        camUp_
    );
}

void Camera::setCenter(float x, float y, float z)
{
    camCenter_.x = x;
    camCenter_.y = y;
    camCenter_.z = z;

    viewMatrix_ = glm::lookAt(
        camPos_,
        camCenter_,
        camUp_
    );
}

glm::vec3 Camera::getUp()
{
    return glm::normalize(glm::cross(getForward(), getRight()));
}


Camera::Camera()
{
    Camera(0.0f, 0.0f, 0.5f);
}

Camera::Camera(float posX, float posY, float posZ)
{
    setPos(posX, posY, posZ);
    viewMatrix_ = glm::lookAt(
        camPos_,
        camCenter_,
        camUp_
    );
}


glm::mat4 Camera::getViewMatrix()
{
    return viewMatrix_;
}

void Camera::setPos(float x, float y, float z)
{
    camPos_.x = x;
    camPos_.y = y;
    camPos_.z = z;
}

void Camera::movePos(float x, float y, float z)
{
    camPos_.x += x;
    camPos_.y += y;
    camPos_.z += z;
}


void Camera::rotateAroundCenter(float angle, glm::vec3 axis)
{
    // TODO: accomodate non-zero center
    glm::mat4 rotMatrix = glm::rotate(glm::mat4(1.0f), angle, axis);
    // camPos_ = rotMatrix*glm::vec4(camPos_,1.0f);
    // printVec(camPos_);

    axis = glm::normalize(axis);
    // build a quaternion for this rotation
    glm::quat q = glm::angleAxis(angle, axis);
    // rotate the position vector
    camPos_ = q * camPos_;

    viewMatrix_ = glm::lookAt(
        camPos_,
        camCenter_,
        camUp_
    );

}

void Camera::changeRadius(float delta)
{
    // printVec(camPos_);
    glm::vec3 centerDir = glm::normalize(camPos_-camCenter_);
    // unstuck center camera
    // if(glm::distance(glm::vec3(0.0f), camPos_)<0.001f)
    // {
    //     std::cout << "too close\n";
    //     centerDir = glm::vec3(0.0f,0.0f,glm::sign(delta));
    // }
    camPos_+=centerDir*delta;

    viewMatrix_ = glm::lookAt(
        camPos_,
        camCenter_,
        camUp_
    );

}


glm::vec3 Camera::getRight()
{
    return glm::cross(getForward(), camUp_);
};

glm::vec3 Camera::getForward()
{
    return glm::normalize(camCenter_-camPos_);
};

