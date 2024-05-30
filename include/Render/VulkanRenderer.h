#pragma once

#include <vulkan/vulkan_core.h>
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <stdexcept>
#include <vector>
#include <set>
#include <algorithm>
#include <array>
#include "stb_image.h"
#include "Mesh.h"
#include "MeshModel.h"
#include "Utilities.h"

class VulkanRenderer
{
public:
	VulkanRenderer();

	int init(GLFWwindow * newWindow);

	void createMeshModel(std::string modelFile, std::string texFile);
	int createTexture(std::string fileName);
	void updateModel(int modelId, glm::mat4 newModel);

	void draw();
	void cleanup();
	void clearScreen();
	void cleanModels();
	
	void recreateSwapChain();


	~VulkanRenderer();

	// Scene Settings
	struct UboViewProjection {
		glm::mat4 projection;
		glm::mat4 view;
	} uboViewProjection;



//Create Light data
	struct LightData{
		glm::vec4 colour[3];
		glm::vec3 position[3];
	} lightData;

	struct MeshPushConstants {
		glm::vec4 push_constant_colour;
		glm::mat4 push_constant_normalMat;
	} pushData;

	std::vector<MeshModel> modelList;

	


	void destroyModels(){
		for(size_t i = 0; i < modelList.size(); i++)
		{
			modelList[i].destroyMeshModel();
		}
	}


private:
	GLFWwindow * window;

	int currentFrame = 0;

	// Scene Objects
	//Multiple meshes 
	std::vector<Mesh> meshList;
	


	// Vulkan Components
	// - Main
	VkInstance instance;
	VkDebugReportCallbackEXT callback;
	struct {
		VkPhysicalDevice physicalDevice;
		VkDevice logicalDevice;
	} mainDevice;
	VkQueue graphicsQueue;
	VkQueue presentationQueue;
	VkSurfaceKHR surface;
	VkSwapchainKHR swapchain;

	std::vector<SwapchainImage> swapChainImages;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	std::vector<VkCommandBuffer> commandBuffers;

	VkImage depthBufferImage;
	VkDeviceMemory depthBufferImageMemory;
	VkImageView depthBufferImageView;

	VkSampler textureSampler;
	// - Descriptors
	//Explains how the data is laid out 
	VkDescriptorSetLayout descriptorSetLayout;	//<------|
												//		 |
	VkPushConstantRange pushConstantRange;		//		 |
												//		 |
	VkDescriptorSetLayout samplerSetLayout;		//		 |
												//		 |
	VkDescriptorPool descriptorPool;			//		 |
	VkDescriptorPool samplerDescriptorPool;     //-------|
	//				/|\
	//				 |
	std::vector<VkDescriptorSet> descriptorSets;
	std::vector<VkDescriptorSet> samplerDescriptorSets;

	//Light data uniform buffer 
	std::vector<VkBuffer> lightUniformBuffer;
	std::vector<VkDeviceMemory> lightUniformBufferMemory;
	
	//View Projection Model, and Light Buffer/Memory
	//Stores the uniform data
	//descriptor set points to buffer to know where its data is 
	std::vector<VkBuffer> vpUniformBuffer;
	//Requires memory for buffer to attach to 
	std::vector<VkDeviceMemory> vpUniformBufferMemory;

	//Model data uniform buffer	
	std::vector<VkBuffer> modelDUniformBuffer;
	std::vector<VkDeviceMemory> modelDUniformBufferMemory;


	
	VkDeviceSize minUniformBufferOffset;
	size_t modelUniformAlignment;
	UboModel * modelTransferSpace;

	// - Assets
	
	std::vector<VkImage> textureImages;
	std::vector<VkDeviceMemory> textureImageMemory;
	std::vector<VkImageView> textureImageView;


	// - Pipeline
	//use a vector
	VkPipeline graphicsPipeline;
	VkPipelineLayout pipelineLayout;
	VkRenderPass renderPass;

	//- Normals PipeLine
	VkPipeline normalsPipeline;
	VkPipelineLayout normalsPipelineLayout;
	

	// - Pools
	VkCommandPool graphicsCommandPool;

	// - Utility
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;

	// - Synchronisation
	std::vector<VkSemaphore> imageAvailable;
	std::vector<VkSemaphore> renderFinished;
	std::vector<VkFence> drawFences;

	// Vulkan Functions
	// - Create Functions
	void createInstance();
	void createLogicalDevice();
	void createSurface();
	void createSwapChain();
	void createRenderPass();
	void createDescriptorSetLayout();

	void createPushConstantRange();	

    void cleanupSwapChain(); 

	void createGraphicsPipeline();
	void createDepthBufferImage();
	void createFramebuffers();
	void createCommandPool();
	void createCommandBuffers();
	void createSynchronisation();
	void createTextureSampler();

	void createUniformBuffers();
	void createDescriptorPool();
	void createDescriptorSets();

	void updateUniformBuffers(uint32_t imageIndex);

	// - Record Functions
	void recordCommands(uint32_t currentIndex);

	// - Get Functions
	void getPhysicalDevice();

	// - Allocate Functions
	void allocateDynamicBufferTransferSpace();

	// - Support Functions
	// -- Checker Functions
	bool checkInstanceExtensionSupport(std::vector<const char*> * checkExtensions);
	bool checkDeviceExtensionSupport(VkPhysicalDevice device);
	bool checkValidationLayerSupport();
	bool checkDeviceSuitable(VkPhysicalDevice device);

	// -- Getter Functions
	QueueFamilyIndices getQueueFamilies(VkPhysicalDevice device);
	SwapChainDetails getSwapChainDetails(VkPhysicalDevice device);

	// -- Choose Functions
	VkSurfaceFormatKHR chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR> &formats);
	VkPresentModeKHR chooseBestPresentationMode(const std::vector<VkPresentModeKHR> presentationModes);
	VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR &surfaceCapabilities);
	VkFormat chooseSupportedFormat(const std::vector<VkFormat> &formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags);



	// -- Create Functions
	VkImage createImage(uint32_t width, 
						uint32_t height, 
						VkFormat format, 
						VkImageTiling tiling, 
						VkImageUsageFlags useFlags, 
						VkMemoryPropertyFlags propFlags, 
						VkDeviceMemory *imageMemory);

	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags);
	VkShaderModule createShaderModule(const std::vector<char> &code);

	int createTextureImage(std::string fileName);
	int createTextureDescriptor(VkImageView textureImage);
	//int createTexture(std::string fileName);
	// -- Loader Functions
	stbi_uc * loadTextureFile(std::string fileName, int* width, int* height, VkDeviceSize* imageSize);

};

