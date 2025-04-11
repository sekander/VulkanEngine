#include "../../include/Camera/Camera.h"

Camera::Camera()
{
    //projectionMatrix = glm::perspective(glm::radians());

    viewMatrix = glm::mat4(1.0f);
}

void Camera::UpdateViewMatrix()
{
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_positon) * 
                                glm::rotate(glm::mat4(1.0f), m_rotation,
                                glm::vec3(0, 0, 1));

    viewMatrix = glm::inverse(transform);    
}

void Camera::RotateAroundPosition(float angle, const glm::vec3& rotationPoint)
{
    // Translate the view matrix to the rotation point
    mat4 translateToRotationPoint = translate(mat4(1.0f), -rotationPoint);

    // Rotate the view matrix around the rotation point
    mat4 rotateAroundRotationPoint = rotate(mat4(1.0f), radians(angle), vec3(0.0f, 1.0f, 0.0f));

    // Translate the view matrix back to the original position
    mat4 translateBack = translate(mat4(1.0f), rotationPoint);

    // Update the view matrix by applying the transformations
    viewMatrix = translateBack * rotateAroundRotationPoint * translateToRotationPoint * viewMatrix;
}
