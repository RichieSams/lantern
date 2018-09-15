/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include "scene/scene.h"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <imgui.h>

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <random>
#include <algorithm>


namespace Lantern {

static const char *const kValidationLayers[] = {
	"VK_LAYER_LUNARG_standard_validation"
};

static const char *const kDeviceExtensions[] = {
	"VK_KHR_swapchain"
};

template <typename T, std::size_t N>
inline std::size_t SizeOfArray(const T(&)[N]) {
	return N;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
	VkDebugReportFlagsEXT flags,
	VkDebugReportObjectTypeEXT objType,
	uint64_t obj,
	size_t location,
	int32_t code,
	const char* layerPrefix,
	const char* msg,
	void* userData) {

	printf("Validation layer: %s\n", msg);

	return VK_FALSE;
}

struct SwapChainSupportDetails {
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentModes;
};

SwapChainSupportDetails QuerySwapChainSupport(vk::PhysicalDevice device, vk::SurfaceKHR surface) {
	SwapChainSupportDetails details;
	details.capabilities = device.getSurfaceCapabilitiesKHR(surface).value;
	details.formats = device.getSurfaceFormatsKHR(surface).value;
	details.presentModes = device.getSurfacePresentModesKHR(surface).value;

	return details;
}



// Needed for message pump callbacks
Visualizer *g_visualizer;

Visualizer::Visualizer(Scene *scene, FrameBuffer *currentFrameBuffer, std::atomic<FrameBuffer *> *swapFrameBuffer)
		: m_scene(scene),
          m_currentFrameBuffer(currentFrameBuffer),
          m_swapFrameBuffer(swapFrameBuffer),
          m_accumulationFrameBuffer(scene->Camera->FrameBufferWidth, scene->Camera->FrameBufferHeight),
		  m_window(nullptr),
		  m_leftMouseCaptured(false),
		  m_middleMouseCaptured(false) {
	g_visualizer = this;
}

Visualizer::~Visualizer() {
}

static void ErrorCallback(int error, const char *description) {
	printf("Error %d: %s\n", error, description);
}

void Visualizer::Run() {
	if (!Init()) {
		Shutdown();
		return;
	}

	auto lastRender = std::chrono::high_resolution_clock::now();
	auto startTime = lastRender;

	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
		
		if (!RenderFrame()) {
			break;
		}
	}

	// Wait for the queues to flush before shutting down
	m_device.waitIdle();

	Shutdown();
}

void Visualizer::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_ALT) {
		glfwGetCursorPos(window, &g_visualizer->m_lastMousePosX, &g_visualizer->m_lastMousePosY);
		g_visualizer->m_leftMouseCaptured = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		g_visualizer->m_leftMouseCaptured = false;
	} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS && mods == GLFW_MOD_ALT) {
		glfwGetCursorPos(window, &g_visualizer->m_lastMousePosX, &g_visualizer->m_lastMousePosY);
		g_visualizer->m_middleMouseCaptured = true;
	} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		g_visualizer->m_middleMouseCaptured = false;
	}

	//g_visualizer->m_imGuiImpl.MouseButtonCallback(window, button, action, mods);
}

void Visualizer::CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
	if (g_visualizer->m_leftMouseCaptured) {
		double oldX = g_visualizer->m_lastMousePosX;
		double oldY = g_visualizer->m_lastMousePosY;
		glfwGetCursorPos(window, &g_visualizer->m_lastMousePosX, &g_visualizer->m_lastMousePosY);

		g_visualizer->m_scene->Camera->Rotate((float)(oldY - g_visualizer->m_lastMousePosY) / 300,
		                                      (float)(oldX - g_visualizer->m_lastMousePosX) / 300);

		g_visualizer->m_accumulationFrameBuffer.Reset();
	} else if (g_visualizer->m_middleMouseCaptured) {
		double oldX = g_visualizer->m_lastMousePosX;
		double oldY = g_visualizer->m_lastMousePosY;
		glfwGetCursorPos(window, &g_visualizer->m_lastMousePosX, &g_visualizer->m_lastMousePosY);

		g_visualizer->m_scene->Camera->Pan((float)(oldX - g_visualizer->m_lastMousePosX) * 0.01f,
		                                   (float)(g_visualizer->m_lastMousePosY - oldY) * 0.01f);

		g_visualizer->m_accumulationFrameBuffer.Reset();
	}
}

void Visualizer::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	g_visualizer->m_scene->Camera->Zoom((float)yoffset);
	g_visualizer->m_accumulationFrameBuffer.Reset();

	//g_visualizer->m_imGuiImpl.ScrollCallback(window, xoffset, yoffset);
}

void Visualizer::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	//g_visualizer->m_imGuiImpl.KeyCallback(window, key, scancode, action, mods);
}

void Visualizer::CharCallback(GLFWwindow *window, uint c) {
	//g_visualizer->m_imGuiImpl.CharCallback(window, c);
}

struct QueueIndices {
	int Graphics = -1;
	int Present = -1;
	int Compute = -1;
	int Transfer = -1;
};

QueueIndices GetQueueIndices(vk::PhysicalDevice *device, vk::SurfaceKHR surface) {
	auto properties = device->getQueueFamilyProperties();

	QueueIndices indices;
	for (int i = 0; i < 1; ++i) {
		if (properties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.Graphics = i;
		} 
		if (properties[i].queueFlags & vk::QueueFlagBits::eCompute) {
			indices.Compute = i;
		} 
		if (properties[i].queueFlags & vk::QueueFlagBits::eTransfer) {
			indices.Transfer = i;
		}

		vk::Bool32 supported = false;
		device->getSurfaceSupportKHR(i, surface, &supported);
		if (supported) {
			indices.Present = i;
		}
	}

	return indices;
}

bool CreateShader(const char *filePath, vk::Device device, vk::ShaderModule *shader) {
	std::ifstream file(filePath, std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		return false;
	}

	size_t fileSize = (size_t)file.tellg();
	void *buffer = malloc(fileSize);

	file.seekg(0);
	file.read((char *)buffer, fileSize);
	file.close();

	vk::ShaderModuleCreateInfo info({}, fileSize, (uint32_t *)buffer);

	vk::Result result = device.createShaderModule(&info, nullptr, shader);
	free(buffer);

	return result == vk::Result::eSuccess;
}

bool Visualizer::Init() {
	// Setup window
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	int width = 1280;// m_scene->Camera->FrameBufferData.Width;
	int height = 720;// m_scene->Camera->FrameBufferData.Height;

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(width, height, "Lantern", nullptr, nullptr);
	m_window = window;

	// Setup ImGui binding
	//m_imGuiImpl.InitImpl(window);

	// Bind callbacks
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCharCallback(window, CharCallback);


	// Set Up Vulkan

	vk::ApplicationInfo appInfo("Lantern", VK_MAKE_VERSION(1, 0, 0), "Lantern", VK_MAKE_VERSION(1, 0, 0), VK_API_VERSION_1_0);

	// Set what extensions we need
	// GLFW needs a few in order to run
	uint glfwExtensionCount = 0;
	const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	std::vector<const char *> extensions;
	for (uint i = 0; i < glfwExtensionCount; ++i) {
		extensions.push_back(glfwExtensions[i]);
	}

	// Now add some of our own
	extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

	// Create the instance
	vk::InstanceCreateInfo instanceInfo({}, &appInfo, SizeOfArray(kValidationLayers), kValidationLayers, extensions.size(), extensions.data());

	vk::Result result = vk::createInstance(&instanceInfo, nullptr, &m_instance);
	if (result != vk::Result::eSuccess) {
		printf("Failed to create Vulkan Instance. Error Code: %d", result);
		return false;
	}

	if (glfwCreateWindowSurface((VkInstance)m_instance, window, nullptr, (VkSurfaceKHR *)&m_surface) != VK_SUCCESS) {
		printf("Failed to create Vulkan Surface. Error Code: %d", result);
		return false;
	}

	vk::DebugReportCallbackCreateInfoEXT debugInfo(vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning, DebugCallback, nullptr);
	m_instance.createDebugReportCallbackEXT(&debugInfo, nullptr, &m_debugCallback);


	auto physicalDevices = m_instance.enumeratePhysicalDevices().value;

	for (vk::PhysicalDevice &device : physicalDevices) {
		vk::PhysicalDeviceProperties deviceProperties;
		vk::PhysicalDeviceFeatures deviceFeatures;

		device.getProperties(&deviceProperties);
		device.getFeatures(&deviceFeatures);

		auto deviceExtensions = device.enumerateDeviceExtensionProperties().value;

		if (deviceProperties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
			m_physicalDevice = device;
			printf("Chose %s\n", deviceProperties.deviceName);
			break;
		}
	}

	// Create the Device and Queues
	QueueIndices indices = GetQueueIndices(&m_physicalDevice, m_surface);

	std::vector<vk::DeviceQueueCreateInfo> queueInfos;
	std::set<int> uniqueIndices = {
		indices.Graphics,
		indices.Present
	};

	float queuePriority = 1.0f;
	for (int index : uniqueIndices) {
		vk::DeviceQueueCreateInfo info({}, index, 1, &queuePriority);
		queueInfos.push_back(info);
	}

	if (indices.Graphics != indices.Present) {
		printf("The device Graphics queue and Present queue are different");
		return false;
	}

	// We don't have any required features yet
	vk::PhysicalDeviceFeatures requiredFeatures = {};
	vk::DeviceCreateInfo deviceInfo({}, queueInfos.size(), queueInfos.data(), SizeOfArray(kValidationLayers), kValidationLayers, SizeOfArray(kDeviceExtensions), kDeviceExtensions, &requiredFeatures);

	result = m_physicalDevice.createDevice(&deviceInfo, nullptr, &m_device);
	if (result != vk::Result::eSuccess) {
		printf("Failed to create Vulkan Device. Error Code: %d", result);
		return false;
	}

	m_device.getQueue(indices.Graphics, 0, &m_graphicsQueue);
	m_device.getQueue(indices.Present, 0, &m_presentQueue);

	// Create the memory allocator
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.physicalDevice = static_cast<VkPhysicalDevice>(m_physicalDevice);
	allocatorCreateInfo.device = static_cast<VkDevice>(m_device);

	vmaCreateAllocator(&allocatorCreateInfo, &m_allocator);


	if (!CreateSwapChain(width, height)) {
		return false;
	}
	if (!CreateImageViews()) {
		return false;
	}
	
	// Create the shaders
	if (!CreateShader("fullscreen_triangle_vs.spv", m_device, &m_vertexShader)) {
		printf("Failed to create the vertex shader: %s", "fullscreen_triangle_vs.spv");
		return false;
	}
	if (!CreateShader("final_resolve_ps.spv", m_device, &m_pixelShader)) {
		printf("Failed to create the pixel shader: %s", "final_resolve_ps.spv");
		return false;
	}

	if (!CreateRenderPass()) {
		return false;
	}
	if (!CreateGraphicsPipeline()) {
		return false;
	}
	if (!CreateFrameBuffers()) {
		return false;
	}

	// Create the staging images
	m_stagingImage.resize(m_frameBuffers.size());
	m_stagingBufferAllocation.resize(m_frameBuffers.size());
	m_stagingBufferAllocInfo.resize(m_frameBuffers.size());
	for (uint i = 0; i < m_frameBuffers.size(); ++i) {
		vk::ImageCreateInfo stagingImageCreateInfo{};
		stagingImageCreateInfo.imageType = vk::ImageType::e2D;
		stagingImageCreateInfo.format = vk::Format::eR8G8B8A8Unorm;
		stagingImageCreateInfo.extent = vk::Extent3D(m_swapchainExtent.width, m_swapchainExtent.height, 1);
		stagingImageCreateInfo.mipLevels = 1;
		stagingImageCreateInfo.arrayLayers = 1;
		stagingImageCreateInfo.samples = vk::SampleCountFlagBits::e1;
		stagingImageCreateInfo.tiling = vk::ImageTiling::eLinear;
		stagingImageCreateInfo.usage = vk::ImageUsageFlagBits::eTransferSrc;
		stagingImageCreateInfo.sharingMode = vk::SharingMode::eExclusive;
		stagingImageCreateInfo.initialLayout = vk::ImageLayout::eTransferSrcOptimal;

		VmaAllocationCreateInfo stagingImageAllocCreateInfo{};
		stagingImageAllocCreateInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		stagingImageAllocCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		if (vmaCreateImage(m_allocator, (VkImageCreateInfo *)&stagingImageCreateInfo, &stagingImageAllocCreateInfo, (VkImage *)&m_stagingImage[i], &m_stagingBufferAllocation[i], &m_stagingBufferAllocInfo[i]) != VK_SUCCESS) {
			printf("Failed to create staging image");
			return false;
		}
	}
	

	// Create the command pool
	vk::CommandPoolCreateInfo commandPoolInfo({}, indices.Graphics);
	if (m_device.createCommandPool(&commandPoolInfo, nullptr, &m_commandPool) != vk::Result::eSuccess) {
		printf("Failed to create the command pool");
		return false;
	}

	if (!CreateCommandBuffers()) {
		return false;
	}

	// Create the semaphores
	vk::SemaphoreCreateInfo semaphoreInfo = {};
	if (m_device.createSemaphore(&semaphoreInfo, nullptr, &m_imageAvailable) != vk::Result::eSuccess) {
		printf("Failed to create Semaphore");
		return false;
	}
	if (m_device.createSemaphore(&semaphoreInfo, nullptr, &m_renderFinished) != vk::Result::eSuccess) {
		printf("Failed to create Semaphore");
		return false;
	}

	return true;
}

void Visualizer::Shutdown() {
	CleanUpSwapChainAndDependents();
	
	for (uint i = 0; i < m_stagingImage.size(); ++i) {
		vmaDestroyImage(m_allocator, (VkImage)m_stagingImage[i], m_stagingBufferAllocation[i]);
	}
	vmaDestroyAllocator(m_allocator);

	m_device.destroySemaphore(m_imageAvailable);
	m_device.destroySemaphore(m_renderFinished);
	m_device.destroyCommandPool(m_commandPool, nullptr);
	m_device.destroyShaderModule(m_vertexShader, nullptr);
	m_device.destroyShaderModule(m_pixelShader, nullptr);
	m_device.destroy(nullptr);

	m_instance.destroyDebugReportCallbackEXT(m_debugCallback, nullptr);
	m_instance.destroySurfaceKHR(m_surface, nullptr);
	m_instance.destroy(nullptr);

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Visualizer::RenderFrame() {
	// Get the next framebuffer to accumulate from
	m_currentFrameBuffer = std::atomic_exchange(m_swapFrameBuffer, m_currentFrameBuffer);

	if (!m_currentFrameBuffer->Empty) {
		for (uint y = 0; y < m_currentFrameBuffer->Height; ++y) {
			const size_t offset = y * m_currentFrameBuffer->Width;
			for (uint x = 0; x < m_currentFrameBuffer->Width; ++x) {
				const size_t index = offset + x;
				m_accumulationFrameBuffer.ColorData[index] += m_currentFrameBuffer->ColorData[index];
				m_accumulationFrameBuffer.Bounces[index] += m_currentFrameBuffer->Bounces[index];
				m_accumulationFrameBuffer.Weights[index] += m_currentFrameBuffer->Weights[index];
			}
		}
		m_currentFrameBuffer->Reset();
	}

	// Let the previous frame's queue flush
	m_presentQueue.waitIdle();

	// Render the frame
	uint32 imageIndex;
	vk::Result result = m_device.acquireNextImageKHR(m_swapchain, std::numeric_limits<uint64_t>::max(), m_imageAvailable, {}, &imageIndex);
	if (result == vk::Result::eErrorOutOfDateKHR) {
		RecreateSwapChainAndDependents();
		return true;
	} else if (result != vk::Result::eSuccess) {
		printf("Failed to acquire next image");
		return false;
	}

	byte *mappedData = (byte *)m_stagingBufferAllocInfo[imageIndex].pMappedData;
	for (uint j = 0; j < m_accumulationFrameBuffer.Height; ++j) {
		const size_t offset = j * m_accumulationFrameBuffer.Width;
		for (uint i = 0; i < m_accumulationFrameBuffer.Width; ++i) {
			const size_t frameBufferIndex = offset + i;
			const size_t mappedDataIndex = frameBufferIndex * 4;

			float3 *color = &m_accumulationFrameBuffer.ColorData[frameBufferIndex];
			float weight = m_accumulationFrameBuffer.Weights[frameBufferIndex];
			mappedData[mappedDataIndex + 0] = (byte)(color->x / weight * 255); // Red
			mappedData[mappedDataIndex + 1] = (byte)(color->y / weight * 255);   // Green
			mappedData[mappedDataIndex + 2] = (byte)(color->z / weight * 255);   // Blue
			mappedData[mappedDataIndex + 3] = (byte)255; // Alpha
		}
	}

	// TODO: Wait on fence that the buffer is finished being transferred from
	//       Outside this. Before we do the memcpy and submit the command buffer

	// Flush to GPU
	vk::MappedMemoryRange flushRange(
		(vk::DeviceMemory)m_stagingBufferAllocInfo[imageIndex].deviceMemory,
		0,
		vk::DeviceSize(VK_WHOLE_SIZE)
	);
	if (m_device.flushMappedMemoryRanges(flushRange) != vk::Result::eSuccess) {
		printf("Failed to flush staging buffer");
		return false;
	}

	vk::PipelineStageFlags waitStages[] = {
		vk::PipelineStageFlagBits::eColorAttachmentOutput
	};
	vk::SubmitInfo submitInfo(1, &m_imageAvailable, waitStages, 1, &m_commandBuffers[imageIndex], 1, &m_renderFinished);

	if (m_graphicsQueue.submit(1, &submitInfo, {}) != vk::Result::eSuccess) {
		printf("Failed to submit draw command buffer");
		return false;
	}

	vk::PresentInfoKHR presentInfo(1, &m_renderFinished, 1, &m_swapchain, &imageIndex, &result);
	result = m_presentQueue.presentKHR(presentInfo);
	if (result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR) {
		RecreateSwapChainAndDependents();
	} else if (result != vk::Result::eSuccess) {
		printf("Failed to present");
		return false;
	}

	return true;
}

bool Visualizer::CreateSwapChain(uint width, uint height) {
	// Set up the swap chain
	SwapChainSupportDetails details = QuerySwapChainSupport(m_physicalDevice, m_surface);

	// Find a suitable surface format
	vk::SurfaceFormatKHR surfaceFormat = {};

	// If the device doesn't care what we choose, just pick the one we want
	if (details.formats.size() == 1 && details.formats[0].format == vk::Format::eUndefined) {
		surfaceFormat.format = vk::Format::eR8G8B8A8Unorm;
		surfaceFormat.colorSpace = vk::ColorSpaceKHR::eAdobergbNonlinearEXT; // sRBG
	} else {
		// Search for the best one
		bool found = false;
		for (vk::SurfaceFormatKHR &surfaceFormat_ : details.formats) {
			if (surfaceFormat_.format == vk::Format::eR8G8B8A8Unorm && surfaceFormat_.colorSpace == vk::ColorSpaceKHR::eAdobergbNonlinearEXT) {
				found = true;
				surfaceFormat = surfaceFormat_;
				break;
			}
		}

		// Just give up and choose the first one
		if (!found) {
			surfaceFormat = details.formats[0];
		}
	}

	// Find a suitable present mode
	vk::PresentModeKHR presentMode = vk::PresentModeKHR::eImmediate;
	for (vk::PresentModeKHR &mode : details.presentModes) {
		if (mode == vk::PresentModeKHR::eMailbox) {
			presentMode = mode;
			// This is the ideal world, so we break
			break;
		} else if (mode == vk::PresentModeKHR::eFifo) {
			presentMode = mode;
			// This is a second best, so we *don't* break, hoping we find eMailbox
		}
	}

	// Choose the best swap extent
	vk::Extent2D swapExtent;
	// If we already have an extent, just use that
	// Otherwise, fall back on the width and height specified
	if (details.capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		swapExtent = details.capabilities.currentExtent;
	} else {
		swapExtent.width = std::max(details.capabilities.minImageExtent.width, std::min(details.capabilities.maxImageExtent.width, width));
		swapExtent.height = std::max(details.capabilities.minImageExtent.height, std::min(details.capabilities.maxImageExtent.height, height));
	}

	uint imageCount = 2;
	if (presentMode == vk::PresentModeKHR::eMailbox) {
		imageCount = 3; // Triple buffering
	}
	// Clamp to the max image count
	if (details.capabilities.maxImageCount > 0 && imageCount > details.capabilities.maxImageCount) {
		imageCount = details.capabilities.maxImageCount;
	}

	// Finally create the swap chain
	vk::SwapchainCreateInfoKHR swapChainInfo;
	swapChainInfo.surface = m_surface;
	swapChainInfo.minImageCount = imageCount;
	swapChainInfo.imageFormat = surfaceFormat.format;
	swapChainInfo.imageColorSpace = surfaceFormat.colorSpace;
	swapChainInfo.imageExtent = swapExtent;
	swapChainInfo.imageArrayLayers = 1;
	swapChainInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
	swapChainInfo.imageSharingMode = vk::SharingMode::eExclusive;
	swapChainInfo.queueFamilyIndexCount = 0;
	swapChainInfo.pQueueFamilyIndices = nullptr;
	swapChainInfo.preTransform = details.capabilities.currentTransform;
	swapChainInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	swapChainInfo.presentMode = presentMode;
	swapChainInfo.clipped = VK_TRUE;
	
	if (m_device.createSwapchainKHR(&swapChainInfo, nullptr, &m_swapchain) != vk::Result::eSuccess) {
		printf("Failed to create the swapchain");
		return false;
	}
	m_swapchainFormat = surfaceFormat.format;
	m_swapchainExtent = swapExtent;

	return true;
}

void Visualizer::CleanUpSwapChainAndDependents() {
	for (vk::Framebuffer &frameBuffer : m_frameBuffers) {
		m_device.destroyFramebuffer(frameBuffer, nullptr);
	}
	m_device.freeCommandBuffers(m_commandPool, m_commandBuffers);
	
	m_device.destroyPipeline(m_mainPipeline, nullptr);
	m_device.destroyPipelineLayout(m_mainPipelineLayout, nullptr);
	m_device.destroyRenderPass(m_renderPass, nullptr);

	for (vk::ImageView &view : m_swapChainImageViews) {
		m_device.destroyImageView(view, nullptr);
	}
	
	m_device.destroySwapchainKHR(m_swapchain, nullptr);
}

bool Visualizer::RecreateSwapChainAndDependents() {
	m_device.waitIdle();

	CleanUpSwapChainAndDependents();

	if (!CreateSwapChain(1, 1)) {
		return false;
	}
	if (!CreateImageViews()) {
		return false;
	}
	if (!CreateRenderPass()) {
		return false;
	}
	if (!CreateGraphicsPipeline()) {
		return false;
	}
	if (!CreateFrameBuffers()) {
		return false;
	}
	if (!CreateCommandBuffers()) {
		return false;
	}

	return true;
}

bool Visualizer::CreateImageViews() {
	// Get the handles for the swapchain image views
	m_swapChainImages = m_device.getSwapchainImagesKHR(m_swapchain).value;

	m_swapChainImageViews.resize(m_swapChainImages.size());
	for (uint i = 0; i < m_swapChainImages.size(); ++i) {
		vk::ImageViewCreateInfo imageViewInfo({}, m_swapChainImages[i], vk::ImageViewType::e2D, m_swapchainFormat, {}, vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1));
		if (m_device.createImageView(&imageViewInfo, nullptr, &m_swapChainImageViews[i]) != vk::Result::eSuccess) {
			printf("Failed to create a swapchain image view");
			return false;
		}
	}

	return true;
}

bool Visualizer::CreateRenderPass() {
	// Create the main render pass

	vk::AttachmentDescription attachments[] = {
		// Backbuffer
		vk::AttachmentDescription(
			{},                                      // flags
			m_swapchainFormat,                       // format
			vk::SampleCountFlagBits::e1,             // samples
			vk::AttachmentLoadOp::eDontCare,         // loadOp
			vk::AttachmentStoreOp::eStore,           // storeOp
			vk::AttachmentLoadOp::eDontCare,         // stencilLoadOp
			vk::AttachmentStoreOp::eDontCare,        // stencilStoreOp
			vk::ImageLayout::eUndefined,             // initialLayout
			vk::ImageLayout::ePresentSrcKHR          // finalLayout
		)
	};

	vk::AttachmentReference colorAttachmentReferences[] = {
		// The backbuffer is a color attachment
		vk::AttachmentReference(
			0,                                          // attachmentIndex
			vk::ImageLayout::eColorAttachmentOptimal    // layout
		)
	};

	vk::SubpassDescription subpasses[] = {
		// Final resolve pass
		vk::SubpassDescription(
			{},                                        // flags
			vk::PipelineBindPoint::eGraphics,          // pipelineBindPoint
			0,                                         // inputAttachmentCount
			nullptr,                                   // pInputAttachments
			SizeOfArray(colorAttachmentReferences),    // colorAttachmentCount
			colorAttachmentReferences,                 // pColorAttachments
			nullptr,                                   // pResolveAttachments
			nullptr,                                   // pDepthStencilAttachment
			0,                                         // preserveAttachmentCount
			nullptr                                    // pPreserveAttachments
		)
	};

	vk::SubpassDependency subpassDependencies[] = {
		// External to final resolve pass
		vk::SubpassDependency(
			VK_SUBPASS_EXTERNAL,                                  // srcSubpass
			0,                                                    // dstSubpass
			vk::PipelineStageFlagBits::eBottomOfPipe,             // srcStageMask
			vk::PipelineStageFlagBits::eTransfer,                 // dstStageMask
			(vk::AccessFlagBits)0,                                // srcAccessMask
			vk::AccessFlagBits::eColorAttachmentWrite,            // dstAccessMask
			vk::DependencyFlagBits::eByRegion                     // dependencyFlags
		),
		// Final resolve to external
		vk::SubpassDependency(
			0,                                                    // srcSubpass
			VK_SUBPASS_EXTERNAL,                                  // dstSubpass
			vk::PipelineStageFlagBits::eColorAttachmentOutput,    // srcStageMask
			vk::PipelineStageFlagBits::eBottomOfPipe,             // dstStageMask
			vk::AccessFlagBits::eColorAttachmentWrite,            // srcAccessMask
			vk::AccessFlagBits::eMemoryRead,                      // dstAccessMask
			vk::DependencyFlagBits::eByRegion                     // dependencyFlags
		)
	};

	vk::RenderPassCreateInfo renderPassInfo(
		{},                                  // flags
		SizeOfArray(attachments),            // attachmentCount
		attachments,                         // pAttachments
		SizeOfArray(subpasses),              // subpassCount
		subpasses,                           // pSubpasses
		SizeOfArray(subpassDependencies),    // dependencyCount
		subpassDependencies                  // pDependencies
	);
	if (m_device.createRenderPass(&renderPassInfo, nullptr, &m_renderPass) != vk::Result::eSuccess) {
		printf("Failed to create render pass");
		return false;
	}

	return true;
}

bool Visualizer::CreateGraphicsPipeline() {
	vk::PipelineShaderStageCreateInfo shaderStageInfos[] = {
		{{}, vk::ShaderStageFlagBits::eVertex, m_vertexShader, "main"},
		{{}, vk::ShaderStageFlagBits::eFragment, m_pixelShader, "main"}
	};

	vk::PipelineVertexInputStateCreateInfo inputStateInfo({}, 0, nullptr, 0, nullptr);
	vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo({}, vk::PrimitiveTopology::eTriangleList, VK_FALSE);

	vk::Viewport viewport(0, 0, m_swapchainExtent.width, m_swapchainExtent.height, 0.0f, 1.0f);
	vk::Rect2D scissor({0, 0}, m_swapchainExtent);
	vk::PipelineViewportStateCreateInfo viewportInfo({}, 1, &viewport, 1, &scissor);

	vk::PipelineRasterizationStateCreateInfo rasterInfo({}, VK_FALSE, VK_FALSE, vk::PolygonMode::eFill, vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise, VK_FALSE, 0.0f, 0.0f, 0.0f, 1.0f);
	vk::PipelineMultisampleStateCreateInfo multisamplingInfo({}, vk::SampleCountFlagBits::e1, VK_FALSE, 1.0f, nullptr, VK_FALSE, VK_FALSE);
	vk::PipelineColorBlendAttachmentState blendAttachment(VK_FALSE,
	                                                      vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
	                                                      vk::BlendFactor::eOne, vk::BlendFactor::eZero, vk::BlendOp::eAdd,
	                                                      vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eA);
	vk::PipelineColorBlendStateCreateInfo blendInfo({}, VK_FALSE, vk::LogicOp::eCopy, 1, &blendAttachment);

	vk::PipelineLayoutCreateInfo pipelineLayoutInfo({}, 0, nullptr, 0, nullptr);
	if (m_device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &m_mainPipelineLayout) != vk::Result::eSuccess) {
		printf("Failed to create the pipeline layout");
		return false;
	}

	// Create the pipeline
	vk::GraphicsPipelineCreateInfo pipelineInfo({}, 2, shaderStageInfos, &inputStateInfo, &inputAssemblyInfo, nullptr, &viewportInfo, &rasterInfo, &multisamplingInfo, nullptr, &blendInfo, nullptr, m_mainPipelineLayout, m_renderPass);
	if (m_device.createGraphicsPipelines({}, 1, &pipelineInfo, nullptr, &m_mainPipeline) != vk::Result::eSuccess) {
		printf("Failed to create pipeline");
		return false;
	}

	return true;
}

bool Visualizer::CreateFrameBuffers() {
	// Create the framebuffers
	m_frameBuffers.resize(m_swapChainImageViews.size());
	for (uint i = 0; i < m_swapChainImageViews.size(); ++i) {
		vk::FramebufferCreateInfo frameBufferInfo({}, m_renderPass, 1, &m_swapChainImageViews[i], m_swapchainExtent.width, m_swapchainExtent.height, 1);
		if (m_device.createFramebuffer(&frameBufferInfo, nullptr, &m_frameBuffers[i]) != vk::Result::eSuccess) {
			printf("Failed to create swapchain framebuffers");
			return false;
		}
	}

	return true;
}

bool Visualizer::CreateCommandBuffers() {
	// Create the command buffers
	m_commandBuffers.resize(m_frameBuffers.size());
	vk::CommandBufferAllocateInfo commandBufferAllocateInfo(m_commandPool, vk::CommandBufferLevel::ePrimary, m_commandBuffers.size());
	if (m_device.allocateCommandBuffers(&commandBufferAllocateInfo, m_commandBuffers.data()) != vk::Result::eSuccess) {
		printf("Failed to allocate the command buffers");
		return false;
	}

	for (uint i = 0; i < m_commandBuffers.size(); ++i) {
		vk::CommandBufferBeginInfo commandBufferBeginInfo({}, nullptr);
		m_commandBuffers[i].begin(commandBufferBeginInfo);

		vk::RenderPassBeginInfo renderPassBeginInfo{};
		renderPassBeginInfo.renderPass = m_renderPass;
		renderPassBeginInfo.framebuffer = m_frameBuffers[i];
		renderPassBeginInfo.renderArea = vk::Rect2D({0, 0}, m_swapchainExtent);
		renderPassBeginInfo.clearValueCount = 0;
		renderPassBeginInfo.pClearValues = nullptr;
		
		m_commandBuffers[i].beginRenderPass(&renderPassBeginInfo, vk::SubpassContents::eInline);
		m_commandBuffers[i].bindPipeline(vk::PipelineBindPoint::eGraphics, m_mainPipeline);

		// Transition image to Transfer Dest Optimal 
		vk::ImageMemoryBarrier imageToTransferSrc(
			(vk::AccessFlagBits)0,                   // srcAccessMask 
			vk::AccessFlagBits::eTransferWrite,      // dstAccessMask 
			vk::ImageLayout::eUndefined,             // oldLayout 
			vk::ImageLayout::eTransferDstOptimal,    // newLayout 
			VK_QUEUE_FAMILY_IGNORED,                 // srcQueueFamilyIndex 
			VK_QUEUE_FAMILY_IGNORED,                 // dstQueueFamilyIndex 
			m_swapChainImages[i],                    // image 
			vk::ImageSubresourceRange(               // subresourceRange 
				vk::ImageAspectFlagBits::eColor,     // aspectMask 
				0,                                   // baseMipLevel 
				1,                                   // levelCount 
				0,                                   // baseArrayLayer 
				1                                    // layerCount 
			)
		);
		m_commandBuffers[i].pipelineBarrier(
			vk::PipelineStageFlagBits::eTopOfPipe,    // srcStageMask 
			vk::PipelineStageFlagBits::eTransfer,     // dstStageMask 
			vk::DependencyFlagBits(),                 // dependencyFlags 
			0,                                        // memoryBarrierCount 
			nullptr,                                  // pMemoryBarriers 
			0,                                        // bufferMemoryBarrierCount 
			nullptr,                                  // pBufferMemoryBarriers 
			1,                                        // imageMemoryBarrierCount 
			&imageToTransferSrc                       // pImageMemoryBarriers 
		);

		vk::ImageBlit blitRegion{};
		blitRegion.srcSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blitRegion.srcSubresource.layerCount = 1;
		blitRegion.srcOffsets[1] = vk::Offset3D(m_swapchainExtent.width, m_swapchainExtent.height, 1);
		blitRegion.dstSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
		blitRegion.dstSubresource.layerCount = 1;
		blitRegion.dstOffsets[1] = vk::Offset3D(m_swapchainExtent.width, m_swapchainExtent.height, 1);
		m_commandBuffers[i].blitImage(m_stagingImage[i], vk::ImageLayout::eTransferSrcOptimal, m_swapChainImages[i], vk::ImageLayout::eTransferDstOptimal, 1, &blitRegion, vk::Filter::eLinear);

		// Transition image to ePresentSrcKHR 
		vk::ImageMemoryBarrier imageToShaderRead(
			vk::AccessFlagBits::eTransferWrite,         // srcAccessMask 
			vk::AccessFlagBits::eMemoryRead,            // dstAccessMask 
			vk::ImageLayout::eTransferDstOptimal,       // oldLayout 
			vk::ImageLayout::ePresentSrcKHR,            // newLayout 
			VK_QUEUE_FAMILY_IGNORED,                    // srcQueueFamilyIndex 
			VK_QUEUE_FAMILY_IGNORED,                    // dstQueueFamilyIndex 
			m_swapChainImages[i],                       // image 
			vk::ImageSubresourceRange(                  // subresourceRange 
				vk::ImageAspectFlagBits::eColor,        // aspectMask 
				0,                                      // baseMipLevel 
				1,                                      // levelCount 
				0,                                      // baseArrayLayer 
				1                                       // layerCount 
			)
		);
		m_commandBuffers[i].pipelineBarrier(
			vk::PipelineStageFlagBits::eTransfer,          // srcStageMask 
			vk::PipelineStageFlagBits::eBottomOfPipe,      // dstStageMask 
			vk::DependencyFlagBits(),                      // dependencyFlags 
			0,                                             // memoryBarrierCount 
			nullptr,                                       // pMemoryBarriers 
			0,                                             // bufferMemoryBarrierCount 
			nullptr,                                       // pBufferMemoryBarriers 
			1,                                             // imageMemoryBarrierCount 
			&imageToShaderRead                             // pImageMemoryBarriers 
		);

		m_commandBuffers[i].endRenderPass();

		if (m_commandBuffers[i].end() != vk::Result::eSuccess) {
			printf("Failed to record command buffer");
			return false;
		}
	}

	return true;
}

} // End of namespace Lantern
