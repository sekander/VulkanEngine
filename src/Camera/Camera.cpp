#include "../../include/Camera/Camera.h"

Camera::Camera()
{
    // Create a perspective projection matrix and flip the Y-axis to fix upside down camera
    projectionMatrix = glm::perspective(glm::radians(45.0f), 16.0f/9.0f, 0.1f, 100.0f);
    projectionMatrix[1][1] *= -1;

    viewMatrix = glm::mat4(1.0f);
    m_rotationQuat = glm::quat(glm::vec3(0.0f)); // Identity rotation
}

void Camera::UpdateViewMatrix()
{
    glm::mat4 rotationMatrix = glm::toMat4(glm::conjugate(m_rotationQuat));
    glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_positon) * 
                                glm::rotate(glm::mat4(1.0f), m_rotation,
                                glm::vec3(1, 0, 0));

    // viewMatrix = glm::inverse(transform);    
    viewMatrix = glm::inverse(transform * rotationMatrix);    
}


void Camera::SetEulerRotation(const glm::vec3& eulerRadians)
{
    // Full Euler rotation: pitch (X), yaw (Y), roll (Z)
    m_rotationQuat = glm::quat(eulerRadians);
    UpdateViewMatrix();
}
void Camera::RotateAroundPosition(float angle, const glm::vec3& rotationPoint)
{
    // Perform a full rotation (360 degrees) around the given point
    float fullRotationAngle = glm::radians(angle); // angle in degrees, e.g., 360.0f for a full rotation

    glm::mat4 translateToRotationPoint = glm::translate(glm::mat4(1.0f), -rotationPoint);
    glm::mat4 rotateAroundRotationPoint = glm::rotate(glm::mat4(1.0f), fullRotationAngle, glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), rotationPoint);

    viewMatrix = translateBack * rotateAroundRotationPoint * translateToRotationPoint * viewMatrix;
}
