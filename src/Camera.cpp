#include "Camera.h"

#include "gtc/matrix_transform.hpp"

Camera::Camera(glm::vec3 position, glm::vec3 up, float yaw, float pitch)
    : front(glm::vec3(0.0f, 0.0f, -1.0f)),
      movementSpeed(SPEED),
      mouseSensitivity(SENSITIVITY),
      fov(FOV)
{
    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;

    UpdateCameraVectors();
}

void Camera::UpdateCameraVectors()
{
    glm::vec3 frontVec;
    frontVec.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    frontVec.y = sin(glm::radians(pitch));
    frontVec.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(frontVec);

    right = glm::normalize(glm::cross(front, worldUp));
    up    = glm::normalize(glm::cross(right, front));
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(position, position + front, up);
}

void Camera::ProcessKeyboard(Camera_Movement direction, float deltaTime)
{
    float velocity = movementSpeed * deltaTime;

    switch (direction)
    {
        case FORWARD:
            position += front * velocity;
            break;
        
        case BACKWARD:
            position -= front * velocity;
            break;

        case LEFT:
            position -= right * velocity;
            break;

        case RIGHT:
            position += right * velocity;
            break;
    }
}

void Camera::ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch)
{
    xOffset *=  mouseSensitivity;
    yOffset *= -mouseSensitivity;

    yaw   += xOffset;
    pitch += yOffset;

    if (constrainPitch)
    {
        if(pitch > 89.0f)  pitch = 89.0f;
        if(pitch < -89.0f) pitch = -89.0f;
    }

    // Update Front, Right, and Up vectors using Euler Angles.
    UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(float yOffset)
{
    fov -= yOffset;
    if (fov < 1.0f)   fov = 1.0f;
    if (fov > 103.0f) fov = 103.0f;
}

glm::vec3 const Camera::GetPosition() const { return position; }

float const Camera::GetFOV() const { return fov; }