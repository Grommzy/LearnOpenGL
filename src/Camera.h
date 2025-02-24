#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>
#include <glad/glad.h>

enum Camera_Movement
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

const float YAW         = -90.0f;
const float PITCH       =  0.0f;
const float SPEED       =  2.5f;
const float SENSITIVITY =  0.1f;
const float FOV         =  90.0f;

class Camera
{
public:
Camera
(
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), 
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), 
    float yaw = YAW, 
    float pitch = PITCH
);

    glm::mat4 GetViewMatrix();

    void ProcessKeyboard(Camera_Movement direction, float deltaTime);

    void ProcessMouseMovement(float xOffset, float yOffset, GLboolean constrainPitch = true);

    void ProcessMouseScroll(float yOffset);

    glm::vec3 const GetPosition() const;
    float const GetFOV() const;

private:
    void UpdateCameraVectors();
    
private:
    glm::vec3 position;
    glm::vec3 front;
    glm::vec3 up;
    glm::vec3 right;
    glm::vec3 worldUp;

    float pitch = 0.0f;
    float yaw = -90.0f;
    float fov = 45.0f;

    float movementSpeed;
    float mouseSensitivity;
};

#endif // CAMERA_H