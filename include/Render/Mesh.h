#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <vector>
#include <string.h>
#include "Utilities.h"

//Add the normal matrix here
//Ubo model
struct UboModel {
	glm::mat4 model;
	glm::mat4 normMat;
	// glm::vec3 colour;
};

class Mesh
{
public:
	Mesh();
	Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, 
		VkQueue transferQueue, VkCommandPool transferCommandPool, 
		std::vector<Vertex> * vertices, std::vector<uint32_t> * indices,
		int newTexID, int normalMapIndex = -1);
	// Mesh(VkPhysicalDevice newPhysicalDevice, VkDevice newDevice, 
	// 	VkQueue transferQueue, VkCommandPool transferCommandPool, 
	// 	std::vector<Vertex> * vertices, std::vector<uint32_t> * indices,
	// 	int newTexID);

	void setModel(glm::mat4 newModel);
	//void setNorm(glm::mat4 model)
	//{
	//	uboModel.normMat = glm::transpose(glm::inverse(model));
	//}

	UboModel getModel();
	void setNormModel(glm::mat4 normModel)
	{
		uboModel.normMat = normModel;
	}

//	void setColour(glm::vec3 colour){uboModel.colour = colour;}

	int getTexID(){return textID;}

	void setTexID(int ID){textID = ID;}

	int getVertexCount();
	VkBuffer getVertexBuffer();

	int getIndexCount();
	VkBuffer getIndexBuffer();

	void destroyBuffers();

	~Mesh();

private:
	UboModel uboModel;

	int textID;
	int normalMapID = -1; // -1 means no normal map

	int vertexCount;
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	int indexCount;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;

	VkPhysicalDevice physicalDevice;
	VkDevice device;

	void createVertexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<Vertex> * vertices);
	void createIndexBuffer(VkQueue transferQueue, VkCommandPool transferCommandPool, std::vector<uint32_t> * indices);
};

