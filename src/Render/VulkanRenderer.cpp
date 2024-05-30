#include "../../include/Render/VulkanRenderer.h"
#include <vulkan/vulkan_core.h>
 
#define _aligned_malloc(size, alignment) aligned_alloc(alignment, size)
#define _aligned_free(ptr) free(ptr)


#include <iostream>

VulkanRenderer::VulkanRenderer()
{
}

int VulkanRenderer::init(GLFWwindow * newWindow)
{
	window = newWindow;

	try {
		createInstance();
		createSurface();
		getPhysicalDevice();
		createLogicalDevice();
		
		createSwapChain();

		createRenderPass();
		createDescriptorSetLayout();
		createPushConstantRange();
		createGraphicsPipeline();
		
		//recreateSwapChain();
		createDepthBufferImage();
		
		createFramebuffers();
			createCommandPool();
			createCommandBuffers();
			createTextureSampler();
			allocateDynamicBufferTransferSpace();
			createUniformBuffers();
			createDescriptorPool();
			createDescriptorSets();
		createSynchronisation();

		
	}
	catch (const std::runtime_error &e) {
		printf("ERROR: %s\n", e.what());
		return EXIT_FAILURE;
	}

	return 0;
}

void VulkanRenderer::updateModel(int modelId, glm::mat4 newModel)
{
	if (modelId >= modelList.size()) return;

	modelList[modelId].setModel(newModel);

	pushData.push_constant_normalMat = glm::transpose(glm::inverse(modelList[modelId].getModel()));	

}

  void VulkanRenderer::cleanupSwapChain() {
	for (auto image : swapChainImages)
	{
		vkDestroyImageView(mainDevice.logicalDevice, image.imageView, nullptr);
	}
	swapChainImages.clear();
	
	if(swapchain != nullptr)
	{
		vkDestroySwapchainKHR(mainDevice.logicalDevice, swapchain, nullptr);
		swapchain = nullptr;
	}

	vkDestroyImageView(mainDevice.logicalDevice, depthBufferImageView, nullptr);
	vkDestroyImage(mainDevice.logicalDevice, depthBufferImage, nullptr);
	vkFreeMemory(mainDevice.logicalDevice, depthBufferImageMemory, nullptr);

	for (auto framebuffer : swapChainFramebuffers)
	{
		vkDestroyFramebuffer(mainDevice.logicalDevice, framebuffer, nullptr);
	}

	vkDestroyRenderPass(mainDevice.logicalDevice, renderPass, nullptr);	

	for (size_t i = 0; i < MAX_FRAME_DRAWS; i++)
	{
		vkDestroySemaphore(mainDevice.logicalDevice, renderFinished[i], nullptr);
		vkDestroySemaphore(mainDevice.logicalDevice, imageAvailable[i], nullptr);
		vkDestroyFence(mainDevice.logicalDevice, drawFences[i], nullptr);
	}

	std::cout << "SWAP CHAIN CLEANED"<< std::endl;
}


void VulkanRenderer::recreateSwapChain()
{
	std::cout << "RECREATE SWAP CHAIN "<< std::endl;

        int width = 0, height = 0;
        //glfwGetFramebufferSize(window, &width, &height);
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(mainDevice.logicalDevice);

        cleanupSwapChain();


        createSwapChain();
			createRenderPass();
			createDepthBufferImage();
        	createFramebuffers();
		createSynchronisation();
		
		createGraphicsPipeline();




	std::cout << "SWAP CHAIN RECREATED"<< std::endl;

}

void VulkanRenderer::draw()
{
	// -- GET NEXT IMAGE --
	// Wait for given fence to signal (open) from last draw before continuing
	vkWaitForFences(mainDevice.logicalDevice, 1, &drawFences[currentFrame], VK_TRUE, std::numeric_limits<uint64_t>::max());
	// Manually reset (close) fences
	vkResetFences(mainDevice.logicalDevice, 1, &drawFences[currentFrame]);
	
	// Get index of next image to be drawn to, and signal semaphore when ready to be drawn to
	uint32_t imageIndex;
	VkResult result = vkAcquireNextImageKHR(mainDevice.logicalDevice, swapchain, std::numeric_limits<uint64_t>::max(), imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex);

	if (result == VK_ERROR_OUT_OF_DATE_KHR  || result == VK_SUBOPTIMAL_KHR ) {
	    recreateSwapChain();
    return;
	} else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
    	throw std::runtime_error("failed to acquire swap chain image!");
	}


	recordCommands(imageIndex);	
	updateUniformBuffers(imageIndex);

	// -- SUBMIT COMMAND BUFFER TO RENDER --
	// Queue submission information
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.waitSemaphoreCount = 1;										// Number of semaphores to wait on
	submitInfo.pWaitSemaphores = &imageAvailable[currentFrame];				// List of semaphores to wait on
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
	};
	submitInfo.pWaitDstStageMask = waitStages;								// Stages to check semaphores at
	submitInfo.commandBufferCount = 1;										// Number of command buffers to submit
	submitInfo.pCommandBuffers = &commandBuffers[imageIndex];				// Command buffer to submit
	submitInfo.signalSemaphoreCount = 1;									// Number of semaphores to signal
	submitInfo.pSignalSemaphores = &renderFinished[currentFrame];			// Semaphores to signal when command buffer finishes

	// Submit command buffer to queue
	//VkResult result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, drawFences[currentFrame]);
	result = vkQueueSubmit(graphicsQueue, 1, &submitInfo, drawFences[currentFrame]);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to submit Command Buffer to Queue!");
	}


	// -- PRESENT RENDERED IMAGE TO SCREEN --
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;										// Number of semaphores to wait on
	presentInfo.pWaitSemaphores = &renderFinished[currentFrame];			// Semaphores to wait on
	presentInfo.swapchainCount = 1;											// Number of swapchains to present to
	presentInfo.pSwapchains = &swapchain;									// Swapchains to present images to
	presentInfo.pImageIndices = &imageIndex;								// Index of images in swapchains to present

	// Present image
	result = vkQueuePresentKHR(presentationQueue, &presentInfo);
	if (result != VK_SUCCESS)
	{
		recreateSwapChain();
		//throw std::runtime_error("Failed to present Image!");
	}

	// Get next frame (use % MAX_FRAME_DRAWS to keep value below MAX_FRAME_DRAWS)
	currentFrame = (currentFrame + 1) % MAX_FRAME_DRAWS;
}

void VulkanRenderer::cleanup()
{
	// Wait until no actions being run on device before destroying
	vkDeviceWaitIdle(mainDevice.logicalDevice);

	for(size_t i = 0; i < modelList.size(); i++)
	{
		modelList[i].destroyMeshModel();
	}

	_aligned_free(modelTransferSpace);

	//Texture Clean up
	vkDestroyDescriptorPool(mainDevice.logicalDevice, samplerDescriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(mainDevice.logicalDevice, samplerSetLayout, nullptr);
	vkDestroySampler(mainDevice.logicalDevice, textureSampler, nullptr);

	for(size_t i = 0; i < textureImages.size(); i++)
	{
		vkDestroyImageView(mainDevice.logicalDevice, textureImageView[i], nullptr);
		vkDestroyImage(mainDevice.logicalDevice, textureImages[i], nullptr);
		vkFreeMemory(mainDevice.logicalDevice, textureImageMemory[i], nullptr);
	}



	vkDestroyImageView(mainDevice.logicalDevice, depthBufferImageView, nullptr);
	vkDestroyImage(mainDevice.logicalDevice, depthBufferImage, nullptr);
	vkFreeMemory(mainDevice.logicalDevice, depthBufferImageMemory, nullptr);

	vkDestroyDescriptorPool(mainDevice.logicalDevice, descriptorPool, nullptr);
	vkDestroyDescriptorSetLayout(mainDevice.logicalDevice, descriptorSetLayout, nullptr);
	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		vkDestroyBuffer(mainDevice.logicalDevice, vpUniformBuffer[i], nullptr);
		vkFreeMemory(mainDevice.logicalDevice, vpUniformBufferMemory[i], nullptr);

	//############################################################################################################
	//############################################################################################################
		vkDestroyBuffer(mainDevice.logicalDevice, lightUniformBuffer[i], nullptr);
		vkFreeMemory(mainDevice.logicalDevice, lightUniformBufferMemory[i], nullptr);
	//############################################################################################################
	//############################################################################################################
		vkDestroyBuffer(mainDevice.logicalDevice, modelDUniformBuffer[i], nullptr);
		vkFreeMemory(mainDevice.logicalDevice, modelDUniformBufferMemory[i], nullptr);
	}
	for (size_t i = 0; i < meshList.size(); i++)
	{
		meshList[i].destroyBuffers();
	}
	for (size_t i = 0; i < MAX_FRAME_DRAWS; i++)
	{
		vkDestroySemaphore(mainDevice.logicalDevice, renderFinished[i], nullptr);
		vkDestroySemaphore(mainDevice.logicalDevice, imageAvailable[i], nullptr);
		vkDestroyFence(mainDevice.logicalDevice, drawFences[i], nullptr);
	}
	vkDestroyCommandPool(mainDevice.logicalDevice, graphicsCommandPool, nullptr);
	for (auto framebuffer : swapChainFramebuffers)
	{
		vkDestroyFramebuffer(mainDevice.logicalDevice, framebuffer, nullptr);
	}
	vkDestroyPipeline(mainDevice.logicalDevice, graphicsPipeline, nullptr);
	vkDestroyPipelineLayout(mainDevice.logicalDevice, pipelineLayout, nullptr);
	vkDestroyRenderPass(mainDevice.logicalDevice, renderPass, nullptr);
	for (auto image : swapChainImages)
	{
		vkDestroyImageView(mainDevice.logicalDevice, image.imageView, nullptr);
	}
	vkDestroySwapchainKHR(mainDevice.logicalDevice, swapchain, nullptr);
	vkDestroySurfaceKHR(instance, surface, nullptr);
	vkDestroyDevice(mainDevice.logicalDevice, nullptr);
	vkDestroyInstance(instance, nullptr);
}


VulkanRenderer::~VulkanRenderer()
{
}

void VulkanRenderer::createInstance()
{

	// Information about the application itself
	// Most data here doesn't affect the program and is for developer convenience
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Vulkan App";							// Custom name of the application
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);				// Custom version of the application
	appInfo.pEngineName = "No Engine";									// Custom engine name
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);					// Custom engine version
	appInfo.apiVersion = VK_API_VERSION_1_0;							// The Vulkan Version

	// Creation information for a VkInstance (Vulkan Instance)
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	// Create list to hold instance extensions
	std::vector<const char*> instanceExtensions = std::vector<const char*>();

	// Set up extensions Instance will use
	uint32_t glfwExtensionCount = 0;									// GLFW may require multiple extensions
	const char** glfwExtensions;										// Extensions passed as array of cstrings, so need pointer (the array) to pointer (the cstring)

	// Get GLFW extensions
	glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	// Add GLFW extensions to list of extensions
	for (size_t i = 0; i < glfwExtensionCount; i++)
	{
		instanceExtensions.push_back(glfwExtensions[i]);
	}

	// If validation enabled, add extension to report validation debug info

	// Check Instance Extensions supported...
	if (!checkInstanceExtensionSupport(&instanceExtensions))
	{
		throw std::runtime_error("VkInstance does not support required extensions!");
	}

	createInfo.enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size());
	createInfo.ppEnabledExtensionNames = instanceExtensions.data();

	createInfo.enabledLayerCount = 0;
	createInfo.ppEnabledLayerNames = nullptr;
	

	// Create instance
	VkResult result = vkCreateInstance(&createInfo, nullptr, &instance);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Vulkan Instance!");
	}
}

void VulkanRenderer::createLogicalDevice()
{
	//Get the queue family indices for the chosen Physical Device
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// Vector for queue creation information, and set for family indices
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> queueFamilyIndices = { indices.graphicsFamily, indices.presentationFamily };

	// Queues the logical device needs to create and info to do so
	for (int queueFamilyIndex : queueFamilyIndices)
	{
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamilyIndex;									// The index of the family to create a queue from
		queueCreateInfo.queueCount = 1;															// Number of queues to create
		float priority = 1.0f;
		queueCreateInfo.pQueuePriorities = &priority;											// Vulkan needs to know how to handle multiple queues, so decide priority (1 = highest priority)

		queueCreateInfos.push_back(queueCreateInfo);
	}

	// Information to create logical device (sometimes called "device")
	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());		// Number of Queue Create Infos
	deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();								// List of queue create infos so device can create required queues
	deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());	// Number of enabled logical device extensions
	deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();							// List of enabled logical device extensions

	// Physical Device Features the Logical Device will be using
	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;													//Enable Anisotropy

	deviceCreateInfo.pEnabledFeatures = &deviceFeatures;										// Physical Device features Logical Device will use
	
	// Create the logical device for the given physical device
	VkResult result = vkCreateDevice(mainDevice.physicalDevice, &deviceCreateInfo, nullptr, &mainDevice.logicalDevice);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Logical Device!");
	}

	// Queues are created at the same time as the device...
	// So we want handle to queues
	// From given logical device, of given Queue Family, of given Queue Index (0 since only one queue), place reference in given VkQueue
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.graphicsFamily, 0, &graphicsQueue);
	vkGetDeviceQueue(mainDevice.logicalDevice, indices.presentationFamily, 0, &presentationQueue);
}

void VulkanRenderer::createSurface()
{
	// Create Surface (creates a surface create info struct, runs the create surface function, returns result)
	VkResult result = glfwCreateWindowSurface(instance, window, nullptr, &surface);

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a surface!");
	}
}

void VulkanRenderer::createSwapChain()
{
	// Get Swap Chain details so we can pick best settings
	SwapChainDetails swapChainDetails = getSwapChainDetails(mainDevice.physicalDevice);

	// Find optimal surface values for our swap chain
	VkSurfaceFormatKHR surfaceFormat = chooseBestSurfaceFormat(swapChainDetails.formats);
	VkPresentModeKHR presentMode = chooseBestPresentationMode(swapChainDetails.presentationModes);
	VkExtent2D extent = chooseSwapExtent(swapChainDetails.surfaceCapabilities);

	// How many images are in the swap chain? Get 1 more than the minimum to allow triple buffering
	uint32_t imageCount = swapChainDetails.surfaceCapabilities.minImageCount + 1;

	// If imageCount higher than max, then clamp down to max
	// If 0, then limitless
	if (swapChainDetails.surfaceCapabilities.maxImageCount > 0 
		&& swapChainDetails.surfaceCapabilities.maxImageCount < imageCount)
	{
		imageCount = swapChainDetails.surfaceCapabilities.maxImageCount;
	}

	// Creation information for swap chain
	VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
	swapChainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	swapChainCreateInfo.surface = surface;																// Swapchain surface
	swapChainCreateInfo.imageFormat = surfaceFormat.format;												// Swapchain format
	swapChainCreateInfo.imageColorSpace = surfaceFormat.colorSpace;										// Swapchain colour space
	swapChainCreateInfo.presentMode = presentMode;														// Swapchain presentation mode
	swapChainCreateInfo.imageExtent = extent;															// Swapchain image extents
	swapChainCreateInfo.minImageCount = imageCount;														// Minimum images in swapchain
	swapChainCreateInfo.imageArrayLayers = 1;															// Number of layers for each image in chain
	swapChainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;								// What attachment images will be used as
	swapChainCreateInfo.preTransform = swapChainDetails.surfaceCapabilities.currentTransform;			// Transform to perform on swap chain images
	swapChainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;								// How to handle blending images with external graphics (e.g. other windows)
	swapChainCreateInfo.clipped = VK_TRUE;																// Whether to clip parts of image not in view (e.g. behind another window, off screen, etc)

	// Get Queue Family Indices
	QueueFamilyIndices indices = getQueueFamilies(mainDevice.physicalDevice);

	// If Graphics and Presentation families are different, then swapchain must let images be shared between families
	if (indices.graphicsFamily != indices.presentationFamily)
	{
		// Queues to share between
		uint32_t queueFamilyIndices[] = {
			(uint32_t)indices.graphicsFamily,
			(uint32_t)indices.presentationFamily
		};

		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;								// Image share handling
		swapChainCreateInfo.queueFamilyIndexCount = 2;													// Number of queues to share images between
		swapChainCreateInfo.pQueueFamilyIndices = queueFamilyIndices;									// Array of queues to share between
	}
	else
	{
		swapChainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		swapChainCreateInfo.queueFamilyIndexCount = 0;
		swapChainCreateInfo.pQueueFamilyIndices = nullptr;
	}

	// IF old swap chain been destroyed and this one replaces it, then link old one to quickly hand over responsibilities
	swapChainCreateInfo.oldSwapchain = VK_NULL_HANDLE;

	// Create Swapchain
	VkResult result = vkCreateSwapchainKHR(mainDevice.logicalDevice, &swapChainCreateInfo, nullptr, &swapchain);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Swapchain!");
	}

	// Store for later reference
	swapChainImageFormat = surfaceFormat.format;
	swapChainExtent = extent;

	// Get swap chain images (first count, then values)
	uint32_t swapChainImageCount;
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapchain, &swapChainImageCount, nullptr);
	std::vector<VkImage> images(swapChainImageCount);
	vkGetSwapchainImagesKHR(mainDevice.logicalDevice, swapchain, &swapChainImageCount, images.data());

	for (VkImage image : images)
	{
		// Store image handle
		SwapchainImage swapChainImage = {};
		swapChainImage.image = image;
		swapChainImage.imageView = createImageView(image, swapChainImageFormat, VK_IMAGE_ASPECT_COLOR_BIT);

		// Add to swapchain image list
		swapChainImages.push_back(swapChainImage);
	}
}

void VulkanRenderer::createRenderPass()
{
	//Attachments
	// Colour attachment of render pass
	VkAttachmentDescription colourAttachment = {};
	colourAttachment.format = swapChainImageFormat;										// Format to use for attachment
	colourAttachment.samples = VK_SAMPLE_COUNT_1_BIT;									// Number of samples to write for multisampling
	colourAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;								// Describes what to do with attachment before rendering
	colourAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;							// Describes what to do with attachment after rendering
	colourAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;					// Describes what to do with stencil before rendering
	colourAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;					// Describes what to do with stencil after rendering

	// Framebuffer data will be stored as an image, but images can be given different data layouts
	// to give optimal use for certain operations
	colourAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;							// Image data layout before render pass starts
	colourAttachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;						// Image data layout after render pass (to change to)


	//Depth Attachments of render pass
	VkAttachmentDescription depthAttachment = {};
	depthAttachment.format = chooseSupportedFormat(
		{VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL, 
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;


	//References
	// Attachment reference uses an attachment index that refers to index in the attachment list passed to renderPassCreateInfo
	VkAttachmentReference colourAttachmentReference = {};
	colourAttachmentReference.attachment = 0;
	colourAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	//Depth Attachment Reference
	VkAttachmentReference depthAttachmentReference = {};
	depthAttachmentReference.attachment = 1;
	depthAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Information about a particular subpass the Render Pass is using
	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;					// Pipeline type subpass is to be bound to
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colourAttachmentReference;
	subpass.pDepthStencilAttachment = &depthAttachmentReference;

	// Need to determine when layout transitions occur using subpass dependencies
	std::array<VkSubpassDependency, 2> subpassDependencies;

	// Conversion from VK_IMAGE_LAYOUT_UNDEFINED to VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
	// Transition must happen after...
	subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;						// Subpass index (VK_SUBPASS_EXTERNAL = Special value meaning outside of renderpass)
	subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;		// Pipeline stage
	subpassDependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;				// Stage access mask (memory access)
	// But must happen before...
	subpassDependencies[0].dstSubpass = 0;
	subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	subpassDependencies[0].dependencyFlags = 0;


	// Conversion from VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	// Transition must happen after...
	subpassDependencies[1].srcSubpass = 0;											
	subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;;
	// But must happen before...
	subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
	subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
	subpassDependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
	subpassDependencies[1].dependencyFlags = 0;


	std::array<VkAttachmentDescription, 2> renderPassAttachments = {colourAttachment, depthAttachment}; 


	// Create info for Render Pass
	VkRenderPassCreateInfo renderPassCreateInfo = {};
	renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(renderPassAttachments.size());
	renderPassCreateInfo.pAttachments = renderPassAttachments.data();
	renderPassCreateInfo.subpassCount = 1;
	renderPassCreateInfo.pSubpasses = &subpass;
	renderPassCreateInfo.dependencyCount = static_cast<uint32_t>(subpassDependencies.size());
	renderPassCreateInfo.pDependencies = subpassDependencies.data();

	VkResult result = vkCreateRenderPass(mainDevice.logicalDevice, &renderPassCreateInfo, nullptr, &renderPass);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Render Pass!");
	}
}

//Create Push Constant Range
void VulkanRenderer::createPushConstantRange()
{
	pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	pushConstantRange.offset = 0;
	//pushConstantRange.size = sizeof(UboModel);
	pushConstantRange.size = sizeof(MeshPushConstants);
}

void VulkanRenderer::createDescriptorSetLayout()
{
	//Uniform Values Descriptor Set Layout


	// UboViewProjection Binding Info
	VkDescriptorSetLayoutBinding vpLayoutBinding = {};
	vpLayoutBinding.binding = 0;													// Binding point in shader (designated by binding number in shader)
	vpLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;				// Type of descriptor (uniform, dynamic uniform, image sampler, etc)
	vpLayoutBinding.descriptorCount = 1;											// Number of descriptors for binding
	vpLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_GEOMETRY_BIT;						// Shader stage to bind to
	vpLayoutBinding.pImmutableSamplers = nullptr;									// For Texture: Can make sampler data unchangeable (immutable) by specifying in layout
//EDIt THIS 

	
	// Model Binding Info
	VkDescriptorSetLayoutBinding modelLayoutBinding = {};
	modelLayoutBinding.binding = 1;
	modelLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	modelLayoutBinding.descriptorCount = 1;
	modelLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	modelLayoutBinding.pImmutableSamplers = nullptr;

	//############################################################################################################
	//############################################################################################################

	// Light Binding Info
	VkDescriptorSetLayoutBinding lightLayoutBinding = {};
	lightLayoutBinding.binding = 2;													// Binding point in shader (designated by binding number in shader)
	lightLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;			// Type of descriptor (uniform, dynamic uniform, image sampler, etc)
	lightLayoutBinding.descriptorCount = 1;											// Number of descriptors for binding
	lightLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;						// Shader stage to bind to
	lightLayoutBinding.pImmutableSamplers = nullptr;								// For Texture: Can make sampler data unchangeable (immutable) by specifying in layout
	
	std::vector<VkDescriptorSetLayoutBinding> layoutBindings = { vpLayoutBinding, modelLayoutBinding, lightLayoutBinding };

	//############################################################################################################
	//############################################################################################################
	
	// Create Descriptor Set Layout with given bindings
	VkDescriptorSetLayoutCreateInfo layoutCreateInfo = {};
	layoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutCreateInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());	// Number of binding infos
	layoutCreateInfo.pBindings = layoutBindings.data();								// Array of binding infos

	// Create Descriptor Set Layout
	VkResult result = vkCreateDescriptorSetLayout(mainDevice.logicalDevice, &layoutCreateInfo, nullptr, &descriptorSetLayout);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Descriptor Set Layout!");
	}

	//Create Texture sampler descriptor set Layout
	//Texture binding info

	VkDescriptorSetLayoutBinding samplerLayoutBinding = {};
	samplerLayoutBinding.binding = 0;
	samplerLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerLayoutBinding.descriptorCount = 1;
	samplerLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	samplerLayoutBinding.pImmutableSamplers = nullptr;

	//Create a Descriptor Set Layout with given bindings for texture
	VkDescriptorSetLayoutCreateInfo textureLayoutCreateInfo = {};
	textureLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	textureLayoutCreateInfo.bindingCount = 1;
	textureLayoutCreateInfo.pBindings = &samplerLayoutBinding;

	//Create Descriptor Set Layout
	result = vkCreateDescriptorSetLayout(mainDevice.logicalDevice, &textureLayoutCreateInfo, nullptr, &samplerSetLayout);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Descriptor Set Layout!");
	}
}

void VulkanRenderer::createDescriptorPool()
{
	//Create Uniform Descriptor Pool


	// Type of descriptors + how many DESCRIPTORS, not Descriptor Sets (combined makes the pool size)
	// ViewProjection Pool
	VkDescriptorPoolSize vpPoolSize = {};
	vpPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	vpPoolSize.descriptorCount = static_cast<uint32_t>(vpUniformBuffer.size());

	//############################################################################################################
	//############################################################################################################
	VkDescriptorPoolSize lightPoolSize = {};
	lightPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	lightPoolSize.descriptorCount = static_cast<uint32_t>(lightUniformBuffer.size());
	//############################################################################################################
	//############################################################################################################
	
	// Model Pool (DYNAMIC)
	VkDescriptorPoolSize modelPoolSize = {};
	modelPoolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	modelPoolSize.descriptorCount = static_cast<uint32_t>(modelDUniformBuffer.size());





	// List of pool sizes
	std::vector<VkDescriptorPoolSize> descriptorPoolSizes = { vpPoolSize, lightPoolSize, modelPoolSize };

	// Data to create Descriptor Pool
	VkDescriptorPoolCreateInfo poolCreateInfo = {};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.maxSets = static_cast<uint32_t>(swapChainImages.size());					// Maximum number of Descriptor Sets that can be created from pool
	poolCreateInfo.poolSizeCount = static_cast<uint32_t>(descriptorPoolSizes.size());		// Amount of Pool Sizes being passed
	poolCreateInfo.pPoolSizes = descriptorPoolSizes.data();									// Pool Sizes to create pool with

	// Create Descriptor Pool
	VkResult result = vkCreateDescriptorPool(mainDevice.logicalDevice, &poolCreateInfo, nullptr, &descriptorPool);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Descriptor Pool!");
	}

	//Create Sampler Descriptor Pool
	//Texture Sampler pool
	VkDescriptorPoolSize samplerPoolSize = {};
	samplerPoolSize.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	samplerPoolSize.descriptorCount = MAX_OBJECTS;

	VkDescriptorPoolCreateInfo samplerPoolCreateInfo = {};
	samplerPoolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	samplerPoolCreateInfo.maxSets = MAX_OBJECTS;
	samplerPoolCreateInfo.poolSizeCount = 1;
	samplerPoolCreateInfo.pPoolSizes = &samplerPoolSize;

	result = vkCreateDescriptorPool(mainDevice.logicalDevice, &samplerPoolCreateInfo, nullptr, &samplerDescriptorPool);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Descriptor Pool!");
	}

}

void VulkanRenderer::createUniformBuffers()
{
	// ViewProjection buffer size
	VkDeviceSize vpBufferSize = sizeof(UboViewProjection);


	//############################################################################################################
	//############################################################################################################
	// Light buffer size
	VkDeviceSize lightBufferSize = sizeof(LightData);
	
	// Model buffer size
	VkDeviceSize modelBufferSize = modelUniformAlignment * MAX_OBJECTS;

	// One uniform buffer for each image (and by extension, command buffer)
	vpUniformBuffer.resize(swapChainImages.size());
	vpUniformBufferMemory.resize(swapChainImages.size());
	
	lightUniformBuffer.resize(swapChainImages.size());
	lightUniformBufferMemory.resize(swapChainImages.size());
	
	modelDUniformBuffer.resize(swapChainImages.size());
	modelDUniformBufferMemory.resize(swapChainImages.size());

	// Create Uniform buffers
	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, vpBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &vpUniformBuffer[i], &vpUniformBufferMemory[i]);

		createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, lightBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &lightUniformBuffer[i], &lightUniformBufferMemory[i]);
		
		createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, modelBufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &modelDUniformBuffer[i], &modelDUniformBufferMemory[i]);
	}
	//############################################################################################################
	//############################################################################################################
}

void VulkanRenderer::createDescriptorSets()
{
	// Resize Descriptor Set list so one for every buffer
	descriptorSets.resize(swapChainImages.size());

	std::vector<VkDescriptorSetLayout> setLayouts(swapChainImages.size(), descriptorSetLayout);

	// Descriptor Set Allocation Info
	VkDescriptorSetAllocateInfo setAllocInfo = {};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.descriptorPool = descriptorPool;									// Pool to allocate Descriptor Set from
	setAllocInfo.descriptorSetCount = static_cast<uint32_t>(swapChainImages.size());// Number of sets to allocate
	setAllocInfo.pSetLayouts = setLayouts.data();									// Layouts to use to allocate sets (1:1 relationship)

	// Allocate descriptor sets (multiple)
	VkResult result = vkAllocateDescriptorSets(mainDevice.logicalDevice, &setAllocInfo, descriptorSets.data());
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Descriptor Sets!");
	}

	// Update all of descriptor set buffer bindings
	for (size_t i = 0; i < swapChainImages.size(); i++)
	{
		// VIEW PROJECTION DESCRIPTOR
		// Buffer info and data offset info
		VkDescriptorBufferInfo vpBufferInfo = {};
		vpBufferInfo.buffer = vpUniformBuffer[i];								// Buffer to get data from
		vpBufferInfo.offset = 0;												// Position of start of data
		vpBufferInfo.range = sizeof(UboViewProjection);							// Size of data

		// Data about connection between binding and buffer
		VkWriteDescriptorSet vpSetWrite = {};
		vpSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		vpSetWrite.dstSet = descriptorSets[i];									// Descriptor Set to update
		vpSetWrite.dstBinding = 0;												// Binding to update (matches with binding on layout/shader)
		vpSetWrite.dstArrayElement = 0;											// Index in array to update
		vpSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;			// Type of descriptor
		vpSetWrite.descriptorCount = 1;											// Amount to update
		vpSetWrite.pBufferInfo = &vpBufferInfo;									// Information about buffer data to bind

	//############################################################################################################
	//############################################################################################################
		// LIGHT DESCRIPTOR
		// Buffer info and data offset info
		VkDescriptorBufferInfo lightBufferInfo = {};
		lightBufferInfo.buffer = lightUniformBuffer[i];							// Buffer to get data from
		lightBufferInfo.offset = 0;												// Position of start of data
		lightBufferInfo.range = sizeof(LightData);								// Size of data

		// Data about connection between binding and buffer
		VkWriteDescriptorSet lightSetWrite = {};
		lightSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		lightSetWrite.dstSet = descriptorSets[i];								// Descriptor Set to update
		lightSetWrite.dstBinding = 2;											// Binding to update (matches with binding on layout/shader)
		lightSetWrite.dstArrayElement = 0;										// Index in array to update
		lightSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;		// Type of descriptor
		lightSetWrite.descriptorCount = 1;										// Amount to update
		lightSetWrite.pBufferInfo = &lightBufferInfo;							// Information about buffer data to bind

	//############################################################################################################
	//############################################################################################################

		// MODEL DESCRIPTOR
		// Model Buffer Binding Info
		VkDescriptorBufferInfo modelBufferInfo = {};
		modelBufferInfo.buffer = modelDUniformBuffer[i];
		modelBufferInfo.offset = 0;
		modelBufferInfo.range = modelUniformAlignment;

		VkWriteDescriptorSet modelSetWrite = {};
		modelSetWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		modelSetWrite.dstSet = descriptorSets[i];
		modelSetWrite.dstBinding = 1;
		modelSetWrite.dstArrayElement = 0;
		modelSetWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		modelSetWrite.descriptorCount = 1;
		modelSetWrite.pBufferInfo = &modelBufferInfo;

		// List of Descriptor Set Writes
		std::vector<VkWriteDescriptorSet> setWrites = { vpSetWrite, lightSetWrite, modelSetWrite };

		// Update the descriptor sets with new buffer/binding info
		vkUpdateDescriptorSets(mainDevice.logicalDevice, static_cast<uint32_t>(setWrites.size()), setWrites.data(), 
								0, nullptr);
	}
}

void VulkanRenderer::updateUniformBuffers(uint32_t imageIndex)
{
	// Copy VP data
	//Crete a random pointer in memory
	void * data;
	vkMapMemory(mainDevice.logicalDevice, vpUniformBufferMemory[imageIndex], 0, sizeof(UboViewProjection), 0, &data);
	memcpy(data, &uboViewProjection, sizeof(UboViewProjection));
	vkUnmapMemory(mainDevice.logicalDevice, vpUniformBufferMemory[imageIndex]);

	//############################################################################################################
	//############################################################################################################
	vkMapMemory(mainDevice.logicalDevice, lightUniformBufferMemory[imageIndex], 0, sizeof(LightData), 0, &data);
	memcpy(data, &lightData, sizeof(LightData));
	vkUnmapMemory(mainDevice.logicalDevice, lightUniformBufferMemory[imageIndex]);
	//############################################################################################################
	//############################################################################################################
	
	// Copy Model data
	for (size_t i = 0; i < modelList.size(); i++)
	{
		UboModel * thisModel = (UboModel *)((uint64_t)modelTransferSpace + (i * modelUniformAlignment));
		thisModel->model = modelList[i].getModel();
	}

	// Map the list of model data
	vkMapMemory(mainDevice.logicalDevice, modelDUniformBufferMemory[imageIndex], 0, modelUniformAlignment * modelList.size(), 0, &data);
	memcpy(data, modelTransferSpace, modelUniformAlignment * modelList.size());
	vkUnmapMemory(mainDevice.logicalDevice, modelDUniformBufferMemory[imageIndex]);
}

void VulkanRenderer::createGraphicsPipeline()
{
	// Read in SPIR-V code of shaders
//	auto vertexShaderCode = readFile("Shaders/vert.spv");
//	auto fragmentShaderCode = readFile("Shaders/frag.spv");
	auto vertexShaderCode = readFile("Shaders/phongVert.spv");
	auto fragmentShaderCode = readFile("Shaders/phongFrag.spv");

//	auto vertexShaderCode = readFile("Shaders/normalVert.spv");
//	auto fragmentShaderCode = readFile("Shaders/normalFrag.spv");

	//Assignment 4 ##############################
	auto geoShaderCode = readFile("Shaders/geo.spv");

  	std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
  	std::vector<VkPipelineShaderStageCreateInfo> normalShaderStages;


	// Create Shader Modules
	VkShaderModule vertexShaderModule = createShaderModule(vertexShaderCode);
	VkShaderModule fragmentShaderModule = createShaderModule(fragmentShaderCode);
	VkShaderModule geoShaderModule = createShaderModule(geoShaderCode);


	// -- SHADER STAGE CREATION INFORMATION --
	// Vertex Stage creation information
	VkPipelineShaderStageCreateInfo vertexShaderCreateInfo = {};
	vertexShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	vertexShaderCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;					// Shader Stage name
	vertexShaderCreateInfo.module = vertexShaderModule;							// Shader module to be used by stage
	vertexShaderCreateInfo.pName = "main";										// Entry point in to shader
	shaderStages.push_back(vertexShaderCreateInfo);

	// Fragment Stage creation information
	VkPipelineShaderStageCreateInfo fragmentShaderCreateInfo = {};
	fragmentShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	fragmentShaderCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;				// Shader Stage name
	fragmentShaderCreateInfo.module = fragmentShaderModule;						// Shader module to be used by stage
	fragmentShaderCreateInfo.pName = "main";									// Entry point in to shader
	shaderStages.push_back(fragmentShaderCreateInfo);

	
	//Geo Shader 
	VkPipelineShaderStageCreateInfo geoShaderCreateInfo = {};
	geoShaderCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	geoShaderCreateInfo.stage = VK_SHADER_STAGE_GEOMETRY_BIT;					// Shader Stage name
	geoShaderCreateInfo.module = geoShaderModule;							// Shader module to be used by stage
	geoShaderCreateInfo.pName = "main";										// Entry point in to shader
//	shaderStages.push_back(geoShaderCreateInfo);
	//normalShaderStages.push_back(geoShaderCreateInfo);
	
	
	
	
	// Put shader stage creation info in to array
	// Graphics Pipeline creation info requires array of shader stage creates
	//VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderCreateInfo, fragmentShaderCreateInfo, geoShaderCreateInfo };
	//VkPipelineShaderStageCreateInfo shaderStages[] = { vertexShaderCreateInfo, fragmentShaderCreateInfo};

	// How the data for a single vertex (including info such as position, colour, texture coords, normals, etc) is as a whole
	VkVertexInputBindingDescription bindingDescription = {};
	bindingDescription.binding = 0;												// Can bind multiple streams of data, this defines which one
	bindingDescription.stride = sizeof(Vertex);									// Size of a single vertex object
	bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;					// How to move between data after each vertex.
																				// VK_VERTEX_INPUT_RATE_INDEX		: Move on to the next vertex
																				// VK_VERTEX_INPUT_RATE_INSTANCE	: Move to a vertex for the next instance

	// How the data for an attribute is defined within a vertex
	std::array<VkVertexInputAttributeDescription, 3> attributeDescriptions;

	// Position Attribute
	attributeDescriptions[0].binding = 0;										// Which binding the data is at (should be same as above)
	attributeDescriptions[0].location = 0;										// Location in shader where data will be read from
	attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;				// Format the data will take (also helps define size of data)
	attributeDescriptions[0].offset = offsetof(Vertex, pos);					// Where this attribute is defined in the data for a single vertex


//EDIT HERE

	// Colour Attribute
	attributeDescriptions[1].binding = 0;							
	attributeDescriptions[1].location = 1;							
	attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;	
	//attributeDescriptions[1].offset = offsetof(Vertex, colour);
	attributeDescriptions[1].offset = offsetof(Vertex, normVert);

	// Texture Attribute
	attributeDescriptions[2].binding = 0;							
	attributeDescriptions[2].location = 2;							
	attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;	
	attributeDescriptions[2].offset = offsetof(Vertex, tex);

	// -- VERTEX INPUT --
	VkPipelineVertexInputStateCreateInfo vertexInputCreateInfo = {};
	vertexInputCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputCreateInfo.vertexBindingDescriptionCount = 1;
	vertexInputCreateInfo.pVertexBindingDescriptions = &bindingDescription;											// List of Vertex Binding Descriptions (data spacing/stride information)
	vertexInputCreateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
	vertexInputCreateInfo.pVertexAttributeDescriptions = attributeDescriptions.data();								// List of Vertex Attribute Descriptions (data format and where to bind to/from)


	// -- INPUT ASSEMBLY --
	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;		// Primitive type to assemble vertices as
	inputAssembly.primitiveRestartEnable = VK_FALSE;					// Allow overriding of "strip" topology to start new primitives


	// -- VIEWPORT & SCISSOR --
	// Create a viewport info struct
	VkViewport viewport = {};
	viewport.x = 0.0f;													// x start coordinate
	viewport.y = 0.0f;													// y start coordinate
	viewport.width = static_cast<float>(swapChainExtent.width);			// width of viewport
	viewport.height = static_cast<float>(swapChainExtent.height);		// height of viewport
	viewport.minDepth = 0.0f;											// min framebuffer depth
	viewport.maxDepth = 1.0f;											// max framebuffer depth

	// Create a scissor info struct
	VkRect2D scissor = {};
	scissor.offset = { 0,0 };											// Offset to use region from
	scissor.extent = swapChainExtent;									// Extent to describe region to use, starting at offset

	VkPipelineViewportStateCreateInfo viewportStateCreateInfo = {};
	viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportStateCreateInfo.viewportCount = 1;
	viewportStateCreateInfo.pViewports = &viewport;
	viewportStateCreateInfo.scissorCount = 1;
	viewportStateCreateInfo.pScissors = &scissor;


	// -- DYNAMIC STATES --
	// Dynamic states to enable
	//std::vector<VkDynamicState> dynamicStateEnables;
	//dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);	// Dynamic Viewport : Can resize in command buffer with vkCmdSetViewport(commandbuffer, 0, 1, &viewport);
	//dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);	// Dynamic Scissor	: Can resize in command buffer with vkCmdSetScissor(commandbuffer, 0, 1, &scissor);

	//// Dynamic State creation info
	//VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo = {};
	//dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	//dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());
	//dynamicStateCreateInfo.pDynamicStates = dynamicStateEnables.data();


	// -- RASTERIZER --
	VkPipelineRasterizationStateCreateInfo rasterizerCreateInfo = {};
	rasterizerCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizerCreateInfo.depthClampEnable = VK_FALSE;					// Change if fragments beyond near/far planes are clipped (default) or clamped to plane
	rasterizerCreateInfo.rasterizerDiscardEnable = VK_FALSE;			// Whether to discard data and skip rasterizer. Never creates fragments, only suitable for pipeline without framebuffer output
	rasterizerCreateInfo.polygonMode = VK_POLYGON_MODE_FILL;			// How to handle filling points between vertices
	rasterizerCreateInfo.lineWidth = 1.0f;								// How thick lines should be when drawn
	rasterizerCreateInfo.cullMode = VK_CULL_MODE_BACK_BIT;				// Which face of a tri to cull
	rasterizerCreateInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;	// Winding to determine which side is front
	rasterizerCreateInfo.depthBiasEnable = VK_FALSE;					// Whether to add depth bias to fragments (good for stopping "shadow acne" in shadow mapping)


	// -- MULTISAMPLING --
	VkPipelineMultisampleStateCreateInfo multisamplingCreateInfo = {};
	multisamplingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisamplingCreateInfo.sampleShadingEnable = VK_FALSE;					// Enable multisample shading or not
	multisamplingCreateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;	// Number of samples to use per fragment


	// -- BLENDING --
	// Blending decides how to blend a new colour being written to a fragment, with the old value

	// Blend Attachment State (how blending is handled)
	VkPipelineColorBlendAttachmentState colourState = {};
	colourState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT	// Colours to apply blending to
		| VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colourState.blendEnable = VK_TRUE;													// Enable blending

	// Blending uses equation: (srcColorBlendFactor * new colour) colorBlendOp (dstColorBlendFactor * old colour)
	colourState.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colourState.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colourState.colorBlendOp = VK_BLEND_OP_ADD;

	// Summarised: (VK_BLEND_FACTOR_SRC_ALPHA * new colour) + (VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA * old colour)
	//			   (new colour alpha * new colour) + ((1 - new colour alpha) * old colour)

	colourState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colourState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colourState.alphaBlendOp = VK_BLEND_OP_ADD;
	// Summarised: (1 * new alpha) + (0 * old alpha) = new alpha

	VkPipelineColorBlendStateCreateInfo colourBlendingCreateInfo = {};
	colourBlendingCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colourBlendingCreateInfo.logicOpEnable = VK_FALSE;				// Alternative to calculations is to use logical operations
	colourBlendingCreateInfo.attachmentCount = 1;
	colourBlendingCreateInfo.pAttachments = &colourState;


	// -- PIPELINE LAYOUT --
	std::array<VkDescriptorSetLayout, 2> descriptorSetLayouts = {descriptorSetLayout, samplerSetLayout};

	//Setup pushconstant with pipeline
	VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
	pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
	pipelineLayoutCreateInfo.pSetLayouts = descriptorSetLayouts.data();
	pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
	pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

	// Create Pipeline Layout
	VkResult result = vkCreatePipelineLayout(mainDevice.logicalDevice, &pipelineLayoutCreateInfo, nullptr, &pipelineLayout);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create Pipeline Layout!");
	}


	// -- DEPTH STENCIL TESTING --
	// TODO: Set up depth stencil testing
	VkPipelineDepthStencilStateCreateInfo depthStencilCreateInfo = {};
	depthStencilCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencilCreateInfo.depthTestEnable = VK_TRUE;											//Enable checking depth to determine fragment write
	depthStencilCreateInfo.depthWriteEnable = VK_TRUE;											//Enable writing to depth buffer (to replace old values)
	depthStencilCreateInfo.depthCompareOp = VK_COMPARE_OP_LESS;									//Comparision operation that allows an overwrite (is in front)
	depthStencilCreateInfo.depthBoundsTestEnable = VK_FALSE;									//Depth Bounds Test: Does the depth value exists between two bounds
	depthStencilCreateInfo.stencilTestEnable = VK_FALSE;										//Enable Stencil test



	// -- GRAPHICS PIPELINE CREATION --
	VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
	pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	//pipelineCreateInfo.stageCount = 3;									// Number of shader stages
	pipelineCreateInfo.stageCount = shaderStages.size();									// Number of shader stages
	pipelineCreateInfo.pStages = shaderStages.data();							// List of shader stages
	pipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;		// All the fixed function pipeline states
	pipelineCreateInfo.pInputAssemblyState = &inputAssembly;
	pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	pipelineCreateInfo.pDynamicState = nullptr;
	pipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
	pipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
	pipelineCreateInfo.pColorBlendState = &colourBlendingCreateInfo;
	pipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
	pipelineCreateInfo.layout = pipelineLayout;							// Pipeline Layout pipeline should use
	pipelineCreateInfo.renderPass = renderPass;							// Render pass description the pipeline is compatible with
	pipelineCreateInfo.subpass = 0;										// Subpass of render pass to use with pipeline

	// Pipeline Derivatives : Can create multiple pipelines that derive from one another for optimisation
	pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;	// Existing pipeline to derive from...
	//pipelineCreateInfo.basePipelineIndex = -1;				// or index of pipeline being created to derive from (in case creating multiple at once)

	// Create Graphics Pipeline
	result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &graphicsPipeline);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Graphics Pipeline!");
	}

	result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr, &normalsPipeline);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Graphics Pipeline!");
	}
	
	// Destroy Shader Modules, no longer needed after Pipeline created
	vkDestroyShaderModule(mainDevice.logicalDevice, fragmentShaderModule, nullptr);
	vkDestroyShaderModule(mainDevice.logicalDevice, vertexShaderModule, nullptr);



/*

	// -- GRAPHICS PIPELINE CREATION --
	VkGraphicsPipelineCreateInfo npipelineCreateInfo = {};
	npipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	//pipelineCreateInfo.stageCount = 3;									// Number of shader stages
	npipelineCreateInfo.stageCount = normalShaderStages.size();									// Number of shader stages
	npipelineCreateInfo.pStages = normalShaderStages.data();							// List of shader stages
	npipelineCreateInfo.pVertexInputState = &vertexInputCreateInfo;		// All the fixed function pipeline states
	npipelineCreateInfo.pInputAssemblyState = &inputAssembly;
	npipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
	npipelineCreateInfo.pDynamicState = nullptr;
	npipelineCreateInfo.pRasterizationState = &rasterizerCreateInfo;
	npipelineCreateInfo.pMultisampleState = &multisamplingCreateInfo;
	npipelineCreateInfo.pColorBlendState = &colourBlendingCreateInfo;
	npipelineCreateInfo.pDepthStencilState = &depthStencilCreateInfo;
	npipelineCreateInfo.layout = normalsPipelineLayout;							// Pipeline Layout pipeline should use
	npipelineCreateInfo.renderPass = renderPass;							// Render pass description the pipeline is compatible with
	npipelineCreateInfo.subpass = 0;										// Subpass of render pass to use with pipeline

	// Pipeline Derivatives : Can create multiple pipelines that derive from one another for optimisation
	npipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;	// Existing pipeline to derive from...
	//npipelineCreateInfo.basePipelineIndex = -1;				// or index of pipeline being created to derive from (in case creating multiple at once)

	// Create Graphics Pipeline
	result = vkCreateGraphicsPipelines(mainDevice.logicalDevice, VK_NULL_HANDLE, 1, &npipelineCreateInfo, nullptr, &normalsPipeline);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Graphics Pipeline!");
	}

*/






















	vkDestroyShaderModule(mainDevice.logicalDevice, geoShaderModule, nullptr);
}

void VulkanRenderer::createDepthBufferImage()
{
	//Get supported format for depth buffer
	VkFormat depthFormat = chooseSupportedFormat(
		{VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT, VK_FORMAT_D24_UNORM_S8_UINT},
		VK_IMAGE_TILING_OPTIMAL, 
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT);

	//Create Depth Buffer Image
	depthBufferImage = createImage(swapChainExtent.width, swapChainExtent.height, depthFormat, VK_IMAGE_TILING_OPTIMAL,
									VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &depthBufferImageMemory);

	//Create Depth buffer Image View
	depthBufferImageView = createImageView(depthBufferImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
}

void VulkanRenderer::createFramebuffers()
{
	// Resize framebuffer count to equal swap chain image count
	swapChainFramebuffers.resize(swapChainImages.size());

	// Create a framebuffer for each swap chain image
	for (size_t i = 0; i < swapChainFramebuffers.size(); i++)
	{
		std::array<VkImageView, 2> attachments = {
			swapChainImages[i].imageView,
			depthBufferImageView
		};

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = renderPass;										// Render Pass layout the Framebuffer will be used with
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
		framebufferCreateInfo.pAttachments = attachments.data();							// List of attachments (1:1 with Render Pass)
		framebufferCreateInfo.width = swapChainExtent.width;								// Framebuffer width
		framebufferCreateInfo.height = swapChainExtent.height;								// Framebuffer height
		framebufferCreateInfo.layers = 1;													// Framebuffer layers

		VkResult result = vkCreateFramebuffer(mainDevice.logicalDevice, &framebufferCreateInfo, nullptr, &swapChainFramebuffers[i]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create a Framebuffer!");
		}
	}
}

void VulkanRenderer::createCommandPool()
{
	// Get indices of queue families from device
	QueueFamilyIndices queueFamilyIndices = getQueueFamilies(mainDevice.physicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;	// Queue Family type that buffers from this command pool will use

	// Create a Graphics Queue Family Command Pool
	VkResult result = vkCreateCommandPool(mainDevice.logicalDevice, &poolInfo, nullptr, &graphicsCommandPool);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a Command Pool!");
	}
}

void VulkanRenderer::createCommandBuffers()
{
	// Resize command buffer count to have one for each framebuffer
	commandBuffers.resize(swapChainFramebuffers.size());

	VkCommandBufferAllocateInfo cbAllocInfo = {};
	cbAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cbAllocInfo.commandPool = graphicsCommandPool;
	cbAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;	// VK_COMMAND_BUFFER_LEVEL_PRIMARY	: Buffer you submit directly to queue. Cant be called by other buffers.
															// VK_COMMAND_BUFFER_LEVEL_SECONARY	: Buffer can't be called directly. Can be called from other buffers via "vkCmdExecuteCommands" when recording commands in primary buffer
	cbAllocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());

	// Allocate command buffers and place handles in array of buffers
	VkResult result = vkAllocateCommandBuffers(mainDevice.logicalDevice, &cbAllocInfo, commandBuffers.data());
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to allocate Command Buffers!");
	}
}

void VulkanRenderer::createSynchronisation()
{
	imageAvailable.resize(MAX_FRAME_DRAWS);
	renderFinished.resize(MAX_FRAME_DRAWS);
	drawFences.resize(MAX_FRAME_DRAWS);

	// Semaphore creation information
	VkSemaphoreCreateInfo semaphoreCreateInfo = {};
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	// Fence creation information
	VkFenceCreateInfo fenceCreateInfo = {};
	fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	for (size_t i = 0; i < MAX_FRAME_DRAWS; i++)
	{
		if (vkCreateSemaphore(mainDevice.logicalDevice, &semaphoreCreateInfo, nullptr, &imageAvailable[i]) != VK_SUCCESS ||
			vkCreateSemaphore(mainDevice.logicalDevice, &semaphoreCreateInfo, nullptr, &renderFinished[i]) != VK_SUCCESS ||
			vkCreateFence(mainDevice.logicalDevice, &fenceCreateInfo, nullptr, &drawFences[i]) != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to create a Semaphore and/or Fence!");
		}
	}
}

void VulkanRenderer::createTextureSampler()
{
	//Sampler Creation info
	VkSamplerCreateInfo samplerCreateInfo = {};
	samplerCreateInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerCreateInfo.magFilter = VK_FILTER_LINEAR;						//How to render when image is magnified on screen
	samplerCreateInfo.minFilter = VK_FILTER_LINEAR;						//How to render when image is minfied on screen
	samplerCreateInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;	//How to handle texture wrap in U (x) direction
	samplerCreateInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;	//How to handle texture wrap in U (x) direction
	samplerCreateInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;	//How to handle texture wrap in U (x) direction
	samplerCreateInfo.borderColor  = VK_BORDER_COLOR_INT_OPAQUE_BLACK;	//Border beyond texture (only works for border clamp)
	samplerCreateInfo.unnormalizedCoordinates = VK_FALSE;				//Whether coords should be normalized (between 0 and 1)
	samplerCreateInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;		//Mipmap interpolatsaion mode
	samplerCreateInfo.mipLodBias = 0.0f;								//Level of Details bias for mip level
	samplerCreateInfo.minLod = 0.0f;									//Minimum level of Detail to pick mip level
	samplerCreateInfo.maxLod = 0.0f;									//Maximum level of Detail to pick mip level
	samplerCreateInfo.anisotropyEnable  = VK_TRUE;						//Enable Anisotropy
	samplerCreateInfo.maxAnisotropy = 16;								//Anisotryopy sample level

	VkResult result = vkCreateSampler(mainDevice.logicalDevice, &samplerCreateInfo, nullptr, &textureSampler);

	if(result != VK_SUCCESS)
		throw std::runtime_error("Failed to create a Texture Sampler");

}



void VulkanRenderer::recordCommands(uint32_t currentIndex)
{
	// Information about how to begin each command buffer
	VkCommandBufferBeginInfo bufferBeginInfo = {};
	bufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	
	// Information about how to begin a render pass (only needed for graphical applications)
	VkRenderPassBeginInfo renderPassBeginInfo = {};
	renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	renderPassBeginInfo.renderPass = renderPass;							// Render Pass to begin
	renderPassBeginInfo.renderArea.offset = { 0, 0 };						// Start point of render pass in pixels
	renderPassBeginInfo.renderArea.extent = swapChainExtent;				// Size of region to run render pass on (starting at offset)
	
	//VkClearValue clearValues[] = {
	std::array<VkClearValue, 2> clearValues = {};
	clearValues[0].color =	{0.0f, 0.0f, 0.0f, 1.0f};
	clearValues[1].depthStencil.depth = 1.0f;
	//};
	renderPassBeginInfo.pClearValues = clearValues.data();							// List of clear values (TODO: Depth Attachment Clear Value)
	renderPassBeginInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());

	renderPassBeginInfo.framebuffer = swapChainFramebuffers[currentIndex];

		// Start recording commands to command buffer!
		VkResult result = vkBeginCommandBuffer(commandBuffers[currentIndex], &bufferBeginInfo);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to start recording a Command Buffer!");
		}

			// Begin Render Pass
			vkCmdBeginRenderPass(commandBuffers[currentIndex], &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

				// Bind Pipeline to be used in render pass
				vkCmdBindPipeline(commandBuffers[currentIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, graphicsPipeline);

				//Iterate through each model object
				for (size_t j = 0; j < modelList.size(); j++)
				{
					MeshModel thisModel = modelList[j];


					for(size_t k = 0; k < thisModel.getMeshCount(); k++)
					{

						VkBuffer vertexBuffers[] = { thisModel.getMesh(k)->getVertexBuffer() };					// Buffers to bind
						VkDeviceSize offsets[] = { 0 };												// Offsets into buffers being bound
						vkCmdBindVertexBuffers(commandBuffers[currentIndex], 0, 1, vertexBuffers, offsets);	// Command to bind vertex buffer before drawing with them

						// Bind mesh index buffer, with 0 offset and using the uint32 type
						vkCmdBindIndexBuffer(commandBuffers[currentIndex],  thisModel.getMesh(k)->getIndexBuffer(), 0, VK_INDEX_TYPE_UINT32);

						// Dynamic Offset Amount
						uint32_t dynamicOffset = static_cast<uint32_t>(modelUniformAlignment) * j;

						//create an array of descriptors sets to be recorded
						std::array<VkDescriptorSet, 2> descriptorSetGroup = {descriptorSets[currentIndex], samplerDescriptorSets[thisModel.getMesh(k)->getTexID()]};

						// Bind Descriptor Sets
						vkCmdBindDescriptorSets(commandBuffers[currentIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, pipelineLayout,
							0, static_cast<uint32_t>(descriptorSetGroup.size()), descriptorSetGroup.data(), 1, &dynamicOffset);

						//Command to push pushConstants to records command
						//Push constants to given shader stage directly
						vkCmdPushConstants(
								commandBuffers[currentIndex],
								pipelineLayout,
								VK_SHADER_STAGE_FRAGMENT_BIT,			//Stage to push constants to
								0,										//Offset of push constant to update
								sizeof(MeshPushConstants),				//Size of data being pushed 
								&pushData								//Actual data being pushed
								//&modelList[j].getModel()	
								//&thisModel
								);


						// Execute pipeline
						vkCmdDrawIndexed(commandBuffers[currentIndex], thisModel.getMesh(k)->getIndexCount(), 1, 0, 0, 0);
					}
				}

			// End Render Pass
			vkCmdEndRenderPass(commandBuffers[currentIndex]);

		// Stop recording to command buffer
		result = vkEndCommandBuffer(commandBuffers[currentIndex]);
		if (result != VK_SUCCESS)
		{
			throw std::runtime_error("Failed to stop recording a Command Buffer!");
		}
}

void VulkanRenderer::getPhysicalDevice()
{
	// Enumerate Physical devices the vkInstance can access
	uint32_t deviceCount = 0;
	vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

	// If no devices available, then none support Vulkan!
	if (deviceCount == 0)
	{
		throw std::runtime_error("Can't find GPUs that support Vulkan Instance!");
	}

	// Get list of Physical Devices
	std::vector<VkPhysicalDevice> deviceList(deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, deviceList.data());

	for (const auto &device : deviceList)
	{
		if (checkDeviceSuitable(device))
		{
			mainDevice.physicalDevice = device;
			break;
		}
	}

	// Get properties of our new device
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(mainDevice.physicalDevice, &deviceProperties);

	minUniformBufferOffset = deviceProperties.limits.minUniformBufferOffsetAlignment;
}

void VulkanRenderer::allocateDynamicBufferTransferSpace()
{
	// Calculate alignment of model data
	modelUniformAlignment = (sizeof(UboModel) + minUniformBufferOffset - 1) 
							& ~(minUniformBufferOffset - 1);

	// Create space in memory to hold dynamic buffer that is aligned to our required alignment and holds MAX_OBJECTS
	modelTransferSpace = (UboModel *)_aligned_malloc(modelUniformAlignment * MAX_OBJECTS, modelUniformAlignment);
}

bool VulkanRenderer::checkInstanceExtensionSupport(std::vector<const char*>* checkExtensions)
{
	// Need to get number of extensions to create array of correct size to hold extensions
	uint32_t extensionCount = 0;
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

	// Create a list of VkExtensionProperties using count
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

	// Check if given extensions are in list of available extensions
	for (const auto &checkExtension : *checkExtensions)
	{
		bool hasExtension = false;
		for (const auto &extension : extensions)
		{
			if (strcmp(checkExtension, extension.extensionName) == 0)
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkDeviceExtensionSupport(VkPhysicalDevice device)
{
	// Get device extension count
	uint32_t extensionCount = 0;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	// If no extensions found, return failure
	if (extensionCount == 0)
	{
		return false;
	}

	// Populate list of extensions
	std::vector<VkExtensionProperties> extensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, extensions.data());

	// Check for extension
	for (const auto &deviceExtension : deviceExtensions)
	{
		bool hasExtension = false;
		for (const auto &extension : extensions)
		{
			if (strcmp(deviceExtension, extension.extensionName) == 0)
			{
				hasExtension = true;
				break;
			}
		}

		if (!hasExtension)
		{
			return false;
		}
	}

	return true;
}

bool VulkanRenderer::checkValidationLayerSupport()
{
	// Get number of validation layers to create vector of appropriate size
	uint32_t validationLayerCount;
	vkEnumerateInstanceLayerProperties(&validationLayerCount, nullptr);

	// Check if no validation layers found AND we want at least 1 layer

	std::vector<VkLayerProperties> availableLayers(validationLayerCount);
	vkEnumerateInstanceLayerProperties(&validationLayerCount, availableLayers.data());

	// Check if given Validation Layer is in list of given Validation Layers

	return true;
}

bool VulkanRenderer::checkDeviceSuitable(VkPhysicalDevice device)
{
	/*
	// Information about the device itself (ID, name, type, vendor, etc)
	VkPhysicalDeviceProperties deviceProperties;
	vkGetPhysicalDeviceProperties(device, &deviceProperties);
	
	*/
	// Information about what the device can do (geo shader, tess shader, wide lines, etc)
	VkPhysicalDeviceFeatures deviceFeatures;
	vkGetPhysicalDeviceFeatures(device, &deviceFeatures);



	QueueFamilyIndices indices = getQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);

	bool swapChainValid = false;
	if (extensionsSupported)
	{
		SwapChainDetails swapChainDetails = getSwapChainDetails(device);
		swapChainValid = !swapChainDetails.presentationModes.empty() && !swapChainDetails.formats.empty();
	}

	return indices.isValid() && extensionsSupported && swapChainValid && deviceFeatures.samplerAnisotropy;
}

QueueFamilyIndices VulkanRenderer::getQueueFamilies(VkPhysicalDevice device)
{
	QueueFamilyIndices indices;

	// Get all Queue Family Property info for the given device
	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

	std::vector<VkQueueFamilyProperties> queueFamilyList(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilyList.data());

	// Go through each queue family and check if it has at least 1 of the required types of queue
	int i = 0;
	for (const auto &queueFamily : queueFamilyList)
	{
		// First check if queue family has at least 1 queue in that family (could have no queues)
		// Queue can be multiple types defined through bitfield. Need to bitwise AND with VK_QUEUE_*_BIT to check if has required type
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
		{
			indices.graphicsFamily = i;		// If queue family is valid, then get index
		}

		// Check if Queue Family supports presentation
		VkBool32 presentationSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentationSupport);
		// Check if queue is presentation type (can be both graphics and presentation)
		if (queueFamily.queueCount > 0 && presentationSupport)
		{
			indices.presentationFamily = i;
		}

		// Check if queue family indices are in a valid state, stop searching if so
		if (indices.isValid())
		{
			break;
		}

		i++;
	}

	return indices;
}

SwapChainDetails VulkanRenderer::getSwapChainDetails(VkPhysicalDevice device)
{
	SwapChainDetails swapChainDetails;

	// -- CAPABILITIES --
	// Get the surface capabilities for the given surface on the given physical device
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &swapChainDetails.surfaceCapabilities);

	// -- FORMATS --
	uint32_t formatCount = 0;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

	// If formats returned, get list of formats
	if (formatCount != 0)
	{
		swapChainDetails.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, swapChainDetails.formats.data());
	}

	// -- PRESENTATION MODES --
	uint32_t presentationCount = 0;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, nullptr);

	// If presentation modes returned, get list of presentation modes
	if (presentationCount != 0)
	{
		swapChainDetails.presentationModes.resize(presentationCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentationCount, swapChainDetails.presentationModes.data());
	}

	return swapChainDetails;
}

// Best format is subjective, but ours will be:
// format		:	VK_FORMAT_R8G8B8A8_UNORM (VK_FORMAT_B8G8R8A8_UNORM as backup)
// colorSpace	:	VK_COLOR_SPACE_SRGB_NONLINEAR_KHR
VkSurfaceFormatKHR VulkanRenderer::chooseBestSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& formats)
{
	// If only 1 format available and is undefined, then this means ALL formats are available (no restrictions)
	if (formats.size() == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
	{
		return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
	}

	// If restricted, search for optimal format
	for (const auto &format : formats)
	{
		if ((format.format == VK_FORMAT_R8G8B8A8_UNORM || format.format == VK_FORMAT_B8G8R8A8_UNORM) 
			&& format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
		{
			return format;
		}
	}

	// If can't find optimal format, then just return first format
	return formats[0];
}

VkPresentModeKHR VulkanRenderer::chooseBestPresentationMode(const std::vector<VkPresentModeKHR> presentationModes)
{
	// Look for Mailbox presentation mode
	for (const auto &presentationMode : presentationModes)
	{
		if (presentationMode == VK_PRESENT_MODE_MAILBOX_KHR)
		{
			return presentationMode;
		}
	}

	// If can't find, use FIFO as Vulkan spec says it must be present
	return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D VulkanRenderer::chooseSwapExtent(const VkSurfaceCapabilitiesKHR & surfaceCapabilities)
{
	// If current extent is at numeric limits, then extent can vary. Otherwise, it is the size of the window.
	if (surfaceCapabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return surfaceCapabilities.currentExtent;
	}
	else
	{
		// If value can vary, need to set manually

		// Get window size
		int width, height;
		glfwGetFramebufferSize(window, &width, &height);

		// Create new extent using window size
		VkExtent2D newExtent = {};
		newExtent.width = static_cast<uint32_t>(width);
		newExtent.height = static_cast<uint32_t>(height);

		// Surface also defines max and min, so make sure within boundaries by clamping value
		newExtent.width = std::max(surfaceCapabilities.minImageExtent.width, std::min(surfaceCapabilities.maxImageExtent.width, newExtent.width));
		newExtent.height = std::max(surfaceCapabilities.minImageExtent.height, std::min(surfaceCapabilities.maxImageExtent.height, newExtent.height));

		return newExtent;
	}
}

VkFormat VulkanRenderer::chooseSupportedFormat(const std::vector<VkFormat> & formats, VkImageTiling tiling, VkFormatFeatureFlags featureFlags)
{
	//Loop through options and find compatible one
	for(VkFormat format : formats)
	{
		//Get properties for give format on this window
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(mainDevice.physicalDevice, format, &properties);

		//Depending on tiling choice, need to check for different bit flag
		if(tiling == VK_IMAGE_TILING_LINEAR && (properties.linearTilingFeatures & featureFlags == featureFlags))
		{
			return format;

		}else if(tiling == VK_IMAGE_TILING_OPTIMAL  && (properties.optimalTilingFeatures & featureFlags == featureFlags))
		{
			return format;
		}
	}

	throw std::runtime_error("Failed to find a mathcing format!");
}

VkImage VulkanRenderer::createImage(uint32_t width, 
						uint32_t height, 
						VkFormat format, 
						VkImageTiling tiling, 
						VkImageUsageFlags useFlags, 
						VkMemoryPropertyFlags propFlags, 
						VkDeviceMemory *imageMemory)
{
	//Create Image

	//Image creation info
	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;					//Type of image (1D, 2D, or 3D)
	imageCreateInfo.extent.width = width;							//Width of image extent
	imageCreateInfo.extent.height = height;							//Height of image extent
	imageCreateInfo.extent.depth = 1;								//Depth of image (just 1, no 3D ascpect)
	imageCreateInfo.mipLevels = 1;									//Number of mipmap levels
	imageCreateInfo.arrayLayers = 1;								//Number of levels in image array
	imageCreateInfo.format = format;								//Format type of image 
	imageCreateInfo.tiling = tiling;								//How image data should be "tiled" (arranged for opt)
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;		//Layout of image data on creation
	imageCreateInfo.usage = useFlags;								//Bit flags definng what image will be used for
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;				//Number of samples for multi-sampling
	imageCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;		//Whether image can be shared between queues

	//Create image
	VkImage image;
	VkResult result = vkCreateImage(mainDevice.logicalDevice, &imageCreateInfo, nullptr, &image);

	if(result != VK_SUCCESS)
		throw std::runtime_error("Failed to create an image!");

	//Create Memory for image

	//Get memory requirments for a type of image
	VkMemoryRequirements memoryRequirements;
	vkGetImageMemoryRequirements(mainDevice.logicalDevice, image,  &memoryRequirements);

	VkMemoryAllocateInfo memoryAllocInfo = {};
	memoryAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	memoryAllocInfo.allocationSize = memoryRequirements.size;
	memoryAllocInfo.memoryTypeIndex = findMemoryTypeIndex(mainDevice.physicalDevice, memoryRequirements.memoryTypeBits, propFlags);

	result = vkAllocateMemory(mainDevice.logicalDevice, &memoryAllocInfo, nullptr, imageMemory);
	
	if(result != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate memory for image");

	//Connect memory to image 
	vkBindImageMemory(mainDevice.logicalDevice, image, *imageMemory, 0);

	return image;

}

VkImageView VulkanRenderer::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags)
{
	VkImageViewCreateInfo viewCreateInfo = {};
	viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewCreateInfo.image = image;											// Image to create view for
	viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;						// Type of image (1D, 2D, 3D, Cube, etc)
	viewCreateInfo.format = format;											// Format of image data
	viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;			// Allows remapping of rgba components to other rgba values
	viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
	viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

	// Subresources allow the view to view only a part of an image
	viewCreateInfo.subresourceRange.aspectMask = aspectFlags;				// Which aspect of image to view (e.g. COLOR_BIT for viewing colour)
	viewCreateInfo.subresourceRange.baseMipLevel = 0;						// Start mipmap level to view from
	viewCreateInfo.subresourceRange.levelCount = 1;							// Number of mipmap levels to view
	viewCreateInfo.subresourceRange.baseArrayLayer = 0;						// Start array level to view from
	viewCreateInfo.subresourceRange.layerCount = 1;							// Number of array levels to view

	// Create image view and return it
	VkImageView imageView;
	VkResult result = vkCreateImageView(mainDevice.logicalDevice, &viewCreateInfo, nullptr, &imageView);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create an Image View!");
	}

	return imageView;
}

VkShaderModule VulkanRenderer::createShaderModule(const std::vector<char>& code)
{
	// Shader Module creation information
	VkShaderModuleCreateInfo shaderModuleCreateInfo = {};
	shaderModuleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	shaderModuleCreateInfo.codeSize = code.size();										// Size of code
	shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t *>(code.data());		// Pointer to code (of uint32_t pointer type)

	VkShaderModule shaderModule;
	VkResult result = vkCreateShaderModule(mainDevice.logicalDevice, &shaderModuleCreateInfo, nullptr, &shaderModule);
	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create a shader module!");
	}

	return shaderModule;
}

int VulkanRenderer::createTextureImage(std::string fileName)
{
	//Load image file
	int width, height;
	VkDeviceSize imageSize;
	stbi_uc * imageData = loadTextureFile(fileName, &width, &height, &imageSize);

	// Create staging buffer to hold loaded data, ready to copy to device
	VkBuffer imageStagingBuffer;
	VkDeviceMemory imageStagingBufferMemory;
	createBuffer(mainDevice.physicalDevice, mainDevice.logicalDevice, imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &imageStagingBuffer, &imageStagingBufferMemory);

	//Copy image data to staging buffer
	//Copies the address pointer 
	void *data;
	vkMapMemory(mainDevice.logicalDevice, imageStagingBufferMemory, 0, imageSize, 0, &data);

	memcpy(data, imageData, static_cast<size_t>(imageSize));
	vkUnmapMemory(mainDevice.logicalDevice, imageStagingBufferMemory);

	//Free original image data
	stbi_image_free(imageData);

	//Create image to hold final texture
	VkImage texImage;
	VkDeviceMemory texImageMemory;

	texImage = createImage(width, height, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, 
								VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &texImageMemory);

	//COPY DATA TO IMAGE
	//Transition image to be DST for copy operation
	transitionImageLayout(mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, texImage, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);	

	//Copy image data
	copyImageBuffer(mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, imageStagingBuffer, texImage, width, height);

	//Transition image to be shader readable for shader usage 
	transitionImageLayout(mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, texImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

	//Add texture data to vector for reference
	textureImages.push_back(texImage);
	textureImageMemory.push_back(texImageMemory);

	//Destroy staging buffers
	vkDestroyBuffer(mainDevice.logicalDevice, imageStagingBuffer, nullptr);
	vkFreeMemory(mainDevice.logicalDevice, imageStagingBufferMemory, nullptr);

	//Return index of new texture image
	return textureImages.size() -1;	
}

int VulkanRenderer::createTexture(std::string fileName)
{
	//Create Texture Image and get its location in array
	int textureImageLoc = createTextureImage(fileName);

	//Create Image View and add to list
	VkImageView imageView = createImageView(textureImages[textureImageLoc], VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
	textureImageView.push_back(imageView);

	//Create Texture Descriptor
	int descriptorLoc = createTextureDescriptor(imageView);

	return descriptorLoc;
}

int VulkanRenderer::createTextureDescriptor(VkImageView textureImage)
{
	VkDescriptorSet descriptorSet;

	//Descriptor Set Allocation Info
	VkDescriptorSetAllocateInfo setAllocInfo = {};
	setAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	setAllocInfo.descriptorPool = samplerDescriptorPool;
	setAllocInfo.descriptorSetCount = 1;
	setAllocInfo.pSetLayouts = &samplerSetLayout;

	//Allocate Descriptor Sets
	VkResult result = vkAllocateDescriptorSets(mainDevice.logicalDevice, &setAllocInfo, &descriptorSet);
	if(result != VK_SUCCESS)
		throw std::runtime_error("Failed to allocate Texture Descriptor Sets ");

	//Texture Image Info
	VkDescriptorImageInfo imageInfo = {};
	imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;		//Image layout when in use
	imageInfo.imageView = textureImage;										//Image to bind to set
	imageInfo.sampler = textureSampler;										//Sampler to use for set


	//Descriptor Write Info
	VkWriteDescriptorSet descriptorWrite = {};
	descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrite.dstSet = descriptorSet;
	descriptorWrite.dstBinding = 0;
	descriptorWrite.dstArrayElement = 0;
	descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrite.descriptorCount = 1;
	descriptorWrite.pImageInfo = &imageInfo;

	//Update new descriptor set
	vkUpdateDescriptorSets(mainDevice.logicalDevice, 1, &descriptorWrite, 0, nullptr);

	//Add descriptor set to list
	samplerDescriptorSets.push_back(descriptorSet);

	//Return descriptor set location

	return samplerDescriptorSets.size() - 1;
}


//void VulkanRenderer::createMeshModel(std::string modelFile){
void VulkanRenderer::createMeshModel(std::string modelFile, std::string texFile){
	
	//Import model scene
	Assimp::Importer importer;
	const aiScene *scene = importer.ReadFile("Models/" + modelFile, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

	if(!scene)
		throw std::runtime_error("Failed to load model! (" + modelFile + ")");

	/*
	//Get vector of all materials with 1:1 ID placement
	//std::vector<std::string> textureNames = MeshModel::LoadMaterials(scene);

	//Conversion from the materials with 1:1 ID placement
	//std::vector<int> matToTex(textureNames.size());

	//Loop over textureNames and create textures for them
	for(size_t i = 0; i < textureNames.size(); i++)
	{
		//If material had no texture, set '0' to indicate no texture, texture 0 will be reserved for a default texture
		if(textureNames[i].empty())
		{
			//matToTex[i] = 0;
			matToTex[i ] - createTexture(texFile);
		}
		else{
			//Otherwise, create texture and set value to index of new texture
			//matToTex[i] - createTexture(textureNames[i]);
			matToTex[i] = 0;
		}
	}
	*/

	auto materialToTex = createTexture(texFile);

	//Load in all our meshes
	//std::vector<Mesh> modelMeshes = MeshModel::LoadNode(mainDevice.physicalDevice, mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, scene->mRootNode, scene, matToTex);
	std::vector<Mesh> modelMeshes = MeshModel::LoadNode(mainDevice.physicalDevice, mainDevice.logicalDevice, graphicsQueue, graphicsCommandPool, scene->mRootNode, scene, materialToTex);

	//Create mesh model and add to list
	MeshModel meshModel = MeshModel(modelMeshes);
	modelList.push_back(meshModel);

}

stbi_uc * VulkanRenderer::loadTextureFile(std::string fileName, int * width, int * height, VkDeviceSize * imageSize)
{
	//Number of channels image uses
	int channels;

	//Load pixel data for image
	std::string fileLoc = "Textures/" + fileName;
	stbi_uc* image = stbi_load(fileLoc.c_str(), width, height, &channels, STBI_rgb_alpha);

	if(!image)
		throw std::runtime_error("Failed to load a texture file (" + fileName + ")");

	//Calculate image size using given and known data
	//4 number of channels
	*imageSize = *width * *height * 4;

	return image;	
}

void VulkanRenderer::clearScreen()
{


// Ensure the command buffer is available
    if (commandBuffers.empty()) {
        throw std::runtime_error("No command buffers available to record commands.");
    }

    // Acquire the next image from the swap chain
    uint32_t imageIndex;
    VkResult result = vkAcquireNextImageKHR(mainDevice.logicalDevice, swapchain, UINT64_MAX, imageAvailable[currentFrame], VK_NULL_HANDLE, &imageIndex);
    if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
        throw std::runtime_error("Failed to acquire swap chain image.");
    }

    // Get the command buffer for the current frame
    VkCommandBuffer commandBuffer = commandBuffers[imageIndex];

    // Begin command buffer recording
    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
        throw std::runtime_error("Failed to begin recording command buffer.");
    }

    // Specify the clear color
    VkClearValue clearColor = { {{ 0.0f, 0.0f, 0.0f, 1.0f }} };

    // Begin render pass
    VkRenderPassBeginInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = renderPass;
    renderPassInfo.framebuffer = swapChainFramebuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = swapChainExtent;
    renderPassInfo.clearValueCount = 1;
    renderPassInfo.pClearValues = &clearColor;

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

    // End render pass
    vkCmdEndRenderPass(commandBuffer);

    // End command buffer recording
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to record command buffer.");
    }

    // Submit the command buffer to the graphics queue
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { imageAvailable[currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;

    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    VkSemaphore signalSemaphores[] = { renderFinished[currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    if (vkQueueSubmit(graphicsQueue, 1, &submitInfo, drawFences[currentFrame]) != VK_SUCCESS) {
        throw std::runtime_error("Failed to submit draw command buffer.");
    }

    // Present the frame
    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = &renderFinished[currentFrame];

    VkSwapchainKHR swapChains[] = { swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &imageIndex;

    vkQueuePresentKHR(presentationQueue, &presentInfo);

    // Wait for the frame to finish
    vkWaitForFences(mainDevice.logicalDevice, 1, &drawFences[currentFrame], VK_TRUE, UINT64_MAX);
    vkResetFences(mainDevice.logicalDevice, 1, &drawFences[currentFrame]);

    currentFrame = (currentFrame + 1) % swapChainImages.size();
	
	

}