#pragma once

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
//#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/quaternion_common.hpp>


#include <glm/gtc/matrix_transform.hpp>


using namespace glm;

class Camera {
private:
    mat4 projectionMatrix;
    mat4 viewMatrix;
	glm::vec3 m_positon;
	glm::vec2 m_scale;
	float m_rotation = 0.0f;



public:
    Camera();
    ~Camera(){}    
		const glm::vec3& GetPosition() const {return m_positon;}
		void SetPosition(const glm::vec3 position) 
		{ 
		  this->m_positon = position;
		  UpdateViewMatrix();	
		}

		float GetRotation() const {return m_rotation;}
		void SetRotation(float rotation)
		{ 
			m_rotation = rotation;
			UpdateViewMatrix();
		}
    
    void UpdateViewMatrix();

	void RotateAroundPosition(float angle, const glm::vec3& rotationPoint);
    mat4 GetProjectionMatrix(){return projectionMatrix;}
    mat4 GetViewMatrix(){return viewMatrix;}

};