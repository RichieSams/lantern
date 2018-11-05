/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include "scene/scene.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

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


// Needed for message pump callbacks
Visualizer *g_visualizer;

Visualizer::Visualizer(Scene *scene, FrameBuffer *currentFrameBuffer, std::atomic<FrameBuffer *> *swapFrameBuffer)
		: m_scene(scene),
          m_currentFrameBuffer(currentFrameBuffer),
          m_swapFrameBuffer(swapFrameBuffer),
          m_accumulationFrameBuffer(scene->Camera->FrameBufferWidth, scene->Camera->FrameBufferHeight),
		  m_window(nullptr) {
	g_visualizer = this;
}

Visualizer::~Visualizer() {
}

static void ErrorCallback(int error, const char *description) {
	printf("Error %d: %s\n", error, description);
}

void Visualizer::Run() {
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
}

void Visualizer::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {

}

void Visualizer::CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {

}

void Visualizer::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {

}

void Visualizer::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {

}

void Visualizer::CharCallback(GLFWwindow *window, uint c) {

}

bool Visualizer::Init(int width, int height) {
	// Setup window
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, 0);
	GLFWwindow *window = glfwCreateWindow(width, height, "Lantern", nullptr, nullptr);
	if (window == nullptr) {
		printf("Failed to create GLFW windows");
		return false;
	}
	m_window = window;

	if (!glfwVulkanSupported()) {
		printf("GLFW: Vulkan not supported");
		return false;
	}

	if (!InitVulkan()) {
		return false;
	}

	// Create window surface
	VkResult createResult = glfwCreateWindowSurface((VkInstance)m_instance, window, nullptr, (VkSurfaceKHR *)&m_surface);
	if (createResult != VK_SUCCESS) {
		printf("Vulkan: Failed to create window surface - Error code: %d", createResult);
		return false;
	}

	int actualWidth;
	int actualHeight;
	glfwGetFramebufferSize(window, &actualWidth, &actualHeight);

	if (!InitVulkanWindow(actualWidth, actualHeight)) {
		return false;
	}

	// Setup ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// Setup GLFW binding
	ImGui_ImplGlfw_InitForVulkan(window, true);

	// Setup Vulkan binding
	ImGui_ImplVulkan_InitInfo initInfo;
	initInfo.Instance = (VkInstance)m_instance;
	initInfo.PhysicalDevice = (VkPhysicalDevice)m_physicalDevice;
	initInfo.Device = (VkDevice)m_device;
	initInfo.QueueFamily = m_graphicsQueueFamilyIndex;
	initInfo.Queue = (VkQueue)m_graphicsQueue;
	initInfo.PipelineCache = VK_NULL_HANDLE;
	initInfo.DescriptorPool = (VkDescriptorPool)m_descriptorPool;
	initInfo.Allocator = nullptr;
	initInfo.CheckVkResultFn = [](VkResult err) {
		if (err == 0) {
			return;
		}
		printf("VkResult %d\n", err);
	};
	ImGui_ImplVulkan_Init(&initInfo, (VkRenderPass)m_renderPass);

	// Setup style
	ImGui::StyleColorsDark();

	// Upload Fonts
	{
		// Use any command queue
		vk::CommandPool commandPool = m_frameData[0].commandPool;
		vk::CommandBuffer commandBuffer = m_frameData[0].commandBuffer;

		vk::Result result = m_device.resetCommandPool(commandPool, {});
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to reset command pool. Error code: %d", result);
			return false;
		}

		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		result = commandBuffer.begin(beginInfo);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to begin command buffer. Error code: %d", result);
			return false;
		}

		ImGui_ImplVulkan_CreateFontsTexture((VkCommandBuffer)commandBuffer);

		vk::SubmitInfo submitInfo;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;

		result = commandBuffer.end();
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to end command buffer. Error code: %d", result);
			return false;
		}
		result = m_graphicsQueue.submit(1, &submitInfo, vk::Fence(nullptr));
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to submit command buffer. Error code: %d", result);
			return false;
		}

		result = m_device.waitIdle();
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to wait idle. Error code: %d", result);
			return false;
		}

		ImGui_ImplVulkan_InvalidateFontUploadObjects();
	}

	return true;
}

void Visualizer::Shutdown() {
	m_device.waitIdle();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//vmaDestroyAllocator(m_allocator);

	m_device.destroyRenderPass(m_renderPass, nullptr);

	for (uint32_t i = 0; i < m_frameCount; ++i) {
		FrameData *data = &m_frameData[i];

		m_device.destroyFramebuffer(data->frameBuffer, nullptr);
		m_device.destroyImageView(data->backbufferView, nullptr);
		m_device.destroyFence(data->fence, nullptr);
		m_device.destroySemaphore(data->imageAcquired, nullptr);
		m_device.destroySemaphore(data->imguiRenderCompleted, nullptr);
		m_device.destroyCommandPool(data->commandPool, nullptr); // This will clean up the associated command buffers
	}
	delete[] m_frameData;

	m_device.destroyDescriptorPool(m_descriptorPool, nullptr);
	m_device.destroySwapchainKHR(m_swapchain, nullptr);
	m_device.destroy(nullptr);
	m_instance.destroyDebugReportCallbackEXT(m_debugCallback, nullptr);
	m_instance.destroySurfaceKHR(m_surface, nullptr);
	m_instance.destroy(nullptr);

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Visualizer::RenderFrame() {
	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
	ImGui::ShowDemoWindow();


	// Rendering

	// We always use the semaphore of the "last" index, since there's no way to know the current index without supplying a semaphore
	vk::Semaphore &imageAcquiredSemaphore = m_frameData[m_frameIndex].imageAcquired;

	vk::Result result = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, imageAcquiredSemaphore, vk::Fence(nullptr), &m_frameIndex);
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to acquire next image. Error code: %d", result);
		return false;
	}


	// Render ImGui to the backbuffer
	ImGui::Render();
	RenderImGui(&imageAcquiredSemaphore);

	vk::PresentInfoKHR info;
	info.waitSemaphoreCount = 1;
	info.pWaitSemaphores = &m_frameData[m_frameIndex].imguiRenderCompleted;
	info.swapchainCount = 1;
	info.pSwapchains = &m_swapchain;
	info.pImageIndices = &m_frameIndex;

	result = m_graphicsQueue.presentKHR(&info);
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to present. Error code: %d", result);
		return false;
	}

	return true;
}

static vk::SurfaceFormatKHR GetSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, const vk::Format* requestedFormats, size_t requestedFormatsCount, const vk::ColorSpaceKHR requestedColorSpace) {
	auto availableFormats = physicalDevice.getSurfaceFormatsKHR(surface).value;

	// First check if only one format, VK_FORMAT_UNDEFINED, is available, which would imply that any format is available
	if (availableFormats.size() == 1) {
		if (availableFormats[0].format == vk::Format::eUndefined) {
			vk::SurfaceFormatKHR surfaceFormat;
			surfaceFormat.format = requestedFormats[0];
			surfaceFormat.colorSpace = requestedColorSpace;

			return surfaceFormat;
		} else {
			// No point in searching another format
			return availableFormats[0];
		}
	} else {
		// Search the for the requested formats in the available formats, the first found will be used
		for (size_t i = 0; i < requestedFormatsCount; ++i) {
			for (auto &&available : availableFormats) {
				if (available.format == requestedFormats[i] && available.colorSpace == requestedColorSpace) {
					return available;
				}
			}
		}

		// If none of the requested image formats could be found, use the first available
		return availableFormats[0];
	}
}

bool Visualizer::InitVulkan() {
	vk::Result result;

	// Create Vulkan instance
	{
		uint glfwExtensionCount = 0;
		const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions;
		for (uint i = 0; i < glfwExtensionCount; ++i) {
			extensions.push_back(glfwExtensions[i]);
		}

		// Now add some of our own
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);

		vk::ApplicationInfo appInfo;
		appInfo.pApplicationName = "Lantern";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "Lantern";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_0;

		vk::InstanceCreateInfo instanceCreateInfo;
		instanceCreateInfo.pApplicationInfo = &appInfo;
		instanceCreateInfo.enabledExtensionCount = extensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = &extensions[0];
		instanceCreateInfo.enabledLayerCount = SizeOfArray(kValidationLayers);
		instanceCreateInfo.ppEnabledLayerNames = kValidationLayers;

		result = vk::createInstance(&instanceCreateInfo, nullptr, &m_instance);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create Vulkan Instance. Error code: %d", result);
			return false;
		}

		vk::DebugReportCallbackCreateInfoEXT debugInfo;
		debugInfo.flags = vk::DebugReportFlagBitsEXT::eError | vk::DebugReportFlagBitsEXT::eWarning | vk::DebugReportFlagBitsEXT::ePerformanceWarning;
		debugInfo.pfnCallback = DebugCallback;

		result = m_instance.createDebugReportCallbackEXT(&debugInfo, nullptr, &m_debugCallback);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to set debug callbacks. Error code: %d", result);
			return false;
		}
	}

	// Select GPU
	{
		auto physicalDevicesResult = m_instance.enumeratePhysicalDevices();
		if (physicalDevicesResult.result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to enumerate physical devices");
			return false;
		}

		// Try to find a discrete GPU
		bool foundDiscrete = false;
		for (vk::PhysicalDevice &device : physicalDevicesResult.value) {
			vk::PhysicalDeviceProperties properties;
			device.getProperties(&properties);

			if (properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
				m_physicalDevice = device;
				foundDiscrete = true;
				break;
			}
		}

		// If we can't find one, just pick the first GPU
		if (!foundDiscrete) {
			m_physicalDevice = physicalDevicesResult.value[0];
		}
	}

	// Select graphics queue family
	{
		m_graphicsQueueFamilyIndex = (uint32_t)-1;

		auto queueFamilyProperties = m_physicalDevice.getQueueFamilyProperties();
		for (size_t i = 0; i < queueFamilyProperties.size(); ++i) {
			if (queueFamilyProperties[i].queueFlags & vk::QueueFlagBits::eGraphics) {
				m_graphicsQueueFamilyIndex = i;
			}
		}

		if (m_graphicsQueueFamilyIndex == (uint32_t)-1) {
			printf("Vulkan: Failed to find a graphics queue");
			return false;
		}
	}

	// Create logical device with 1 queue
	{
		const float queuePriority[] = { 1.0f };

		vk::DeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = queuePriority;

		vk::DeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.enabledExtensionCount = SizeOfArray(kDeviceExtensions);
		deviceCreateInfo.ppEnabledExtensionNames = kDeviceExtensions;
	
		result = m_physicalDevice.createDevice(&deviceCreateInfo, nullptr, &m_device);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create logical device. Error code: %d", result);
			return false;
		}

		m_device.getQueue(m_graphicsQueueFamilyIndex, 0, &m_graphicsQueue);
	}

	// Create descriptor pool
	{
		vk::DescriptorPoolSize poolSizes[] = {
			{vk::DescriptorType::eSampler, 1000},
			{vk::DescriptorType::eCombinedImageSampler, 1000},
			{vk::DescriptorType::eSampledImage, 1000},
			{vk::DescriptorType::eStorageImage, 1000},
			{vk::DescriptorType::eUniformTexelBuffer, 1000},
			{vk::DescriptorType::eStorageTexelBuffer, 1000},
			{vk::DescriptorType::eUniformBuffer, 1000},
			{vk::DescriptorType::eStorageBuffer, 1000},
			{vk::DescriptorType::eUniformBufferDynamic, 1000},
			{vk::DescriptorType::eStorageBufferDynamic, 1000},
			{vk::DescriptorType::eInputAttachment, 1000},
		};

		vk::DescriptorPoolCreateInfo createInfo;
		createInfo.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		createInfo.maxSets = 1000 * SizeOfArray(poolSizes);
		createInfo.poolSizeCount = SizeOfArray(poolSizes);
		createInfo.pPoolSizes = poolSizes;

		result = m_device.createDescriptorPool(&createInfo, nullptr, &m_descriptorPool);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create descriptor pool. Error code: %d", result);
			return false;
		}
	}

	return true;
}

bool Visualizer::InitVulkanWindow(int width, int height) {
	vk::Result result;

	// Check for WSI support
	{
		vk::Bool32 supported = false;
		result = m_physicalDevice.getSurfaceSupportKHR(m_graphicsQueueFamilyIndex, m_surface, &supported);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to query for surface support - Error code: %d", result);
			return false;
		}
		if (supported != VK_TRUE) {
			printf("Vulkan: No WSI support on physical device");
			return false;
		}
	}

	// Get Surface format
	{
		const vk::Format requestedFormats[] = {
			vk::Format::eB8G8R8A8Unorm,
			vk::Format::eR8G8B8A8Unorm,
			vk::Format::eB8G8R8Unorm,
			vk::Format::eR8G8B8Unorm,
		};
		const vk::ColorSpaceKHR requestedColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;

		m_surfaceFormat = GetSurfaceFormat(m_physicalDevice, m_surface, requestedFormats, SizeOfArray(requestedFormats), requestedColorSpace);
	}

	// Present mode
	// FIFO is required by Vulkan spec
	m_presentMode = vk::PresentModeKHR::eFifo;
	m_frameCount = 2;

	// Create swapchain
	CreateSwapChain(width, height);

	// Create per frame data structures
	{
		for (uint32_t i = 0; i < m_frameCount; i++) {
			// Create Command pool
			{
				vk::CommandPoolCreateInfo info;
				info.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
				info.queueFamilyIndex = m_graphicsQueueFamilyIndex;

				result = m_device.createCommandPool(&info, nullptr, &m_frameData[i].commandPool);
				if (result != vk::Result::eSuccess) {
					printf("Vulkan: Failed to create command pool. Error code: %d", result);
					return false;
				}
			}
			// Create command buffer
			{
				vk::CommandBufferAllocateInfo info;
				info.commandPool = m_frameData[i].commandPool;
				info.commandBufferCount = 1;
				info.level = vk::CommandBufferLevel::ePrimary;

				result = m_device.allocateCommandBuffers(&info, &m_frameData[i].commandBuffer);
				if (result != vk::Result::eSuccess) {
					printf("Vulkan: Failed to create command buffer. Error code: %d", result);
					return false;
				}
			}
			// Create fence
			{
				vk::FenceCreateInfo info;
				info.flags = vk::FenceCreateFlagBits::eSignaled;

				result = m_device.createFence(&info, nullptr, &m_frameData[i].fence);
				if (result != vk::Result::eSuccess) {
					printf("Vulkan: Failed to create fence. Error code: %d", result);
					return false;
				}
			}
			// Create semaphores
			{
				vk::SemaphoreCreateInfo info;

				result = m_device.createSemaphore(&info, nullptr, &m_frameData[i].imageAcquired);
				if (result != vk::Result::eSuccess) {
					printf("Vulkan: Failed to create imageAcquired semaphore. Error code: %d", result);
					return false;
				}

				result = m_device.createSemaphore(&info, nullptr, &m_frameData[i].imguiRenderCompleted);
				if (result != vk::Result::eSuccess) {
					printf("Vulkan: Failed to create renderCompleted semaphore. Error code: %d", result);
					return false;
				}
			}
		}
	}

	// Create the Render Pass
	{
		vk::AttachmentDescription attachment;
		attachment.format = m_surfaceFormat.format;
		attachment.samples = vk::SampleCountFlagBits::e1;
		attachment.loadOp = vk::AttachmentLoadOp::eDontCare;
		attachment.storeOp = vk::AttachmentStoreOp::eStore;
		attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		attachment.initialLayout = vk::ImageLayout::eUndefined;
		attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

		vk::AttachmentReference colorAttachment;
		colorAttachment.attachment = 0;
		colorAttachment.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass;
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachment;

		vk::SubpassDependency dependency;
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependency.srcAccessMask = vk::AccessFlags();
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;

		vk::RenderPassCreateInfo info;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;

		result = m_device.createRenderPass(&info, nullptr, &m_renderPass);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create render pass. Error code: %d", result);
			return false;
		}
	}

	// Create the backbuffer Views
	{
		vk::ImageViewCreateInfo info;
		info.viewType = vk::ImageViewType::e2D;
		info.format = m_surfaceFormat.format;
		info.components.r = vk::ComponentSwizzle::eR;
		info.components.g = vk::ComponentSwizzle::eG;
		info.components.b = vk::ComponentSwizzle::eB;
		info.components.a = vk::ComponentSwizzle::eA;

		vk::ImageSubresourceRange imageRange;
		imageRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		imageRange.baseMipLevel = 0;
		imageRange.levelCount = 1;
		imageRange.baseArrayLayer = 0;
		imageRange.layerCount = 1;

		info.subresourceRange = imageRange;

		for (uint32_t i = 0; i < m_frameCount; ++i) {
			info.image = m_frameData[i].backbuffer;
			result = m_device.createImageView(&info, nullptr, &m_frameData[i].backbufferView);
			if (result != vk::Result::eSuccess) {
				printf("Vulkan: Failed to create backbuffer image view. Error code: %d", result);
				return false;
			}
		}
	}

	// Create Framebuffer
	{
		vk::FramebufferCreateInfo info;
		info.renderPass = m_renderPass;
		info.attachmentCount = 1;
		// info.pAttachements is set in the for loop below
		info.width = width;
		info.height = height;
		info.layers = 1;
		
		for (uint32_t i = 0; i < m_frameCount; i++) {
			info.pAttachments = &m_frameData[i].backbufferView;
			result = m_device.createFramebuffer(&info, nullptr, &m_frameData[i].frameBuffer);
			if (result != vk::Result::eSuccess) {
				printf("Vulkan: Failed to create framebuffer. Error code: %d", result);
				return false;
			}
		}
	}

	m_frameIndex = 0;

	return true;
}

static int GetMinImageCount(vk::PresentModeKHR mode) {
	if (mode == vk::PresentModeKHR::eMailbox)
		return 3;
	if (mode == vk::PresentModeKHR::eFifo || mode == vk::PresentModeKHR::eFifoRelaxed)
		return 2;
	if (mode == vk::PresentModeKHR::eImmediate)
		return 1;
	assert(false);
	return 1;
}

bool Visualizer::CreateSwapChain(int width, int height) {
	vk::SwapchainKHR oldSwapchain = m_swapchain;

	vk::SwapchainCreateInfoKHR info;
	info.surface = m_surface;
	info.minImageCount = GetMinImageCount(m_presentMode);
	info.imageFormat = m_surfaceFormat.format;
	info.imageColorSpace = m_surfaceFormat.colorSpace;
	info.imageArrayLayers = 1;
	info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
	info.imageSharingMode = vk::SharingMode::eExclusive;
	info.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
	info.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	info.presentMode = m_presentMode;
	info.clipped = VK_TRUE;
	info.oldSwapchain = m_swapchain;

	vk::SurfaceCapabilitiesKHR capabilities;
	vk::Result result = m_physicalDevice.getSurfaceCapabilitiesKHR(m_surface, &capabilities);
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to fetch surface capabilities. Error code: %d", result);
		return false;
	}

	if (info.minImageCount < capabilities.minImageCount) {
		info.minImageCount = capabilities.minImageCount;
	} else if (capabilities.maxImageCount != 0 && info.minImageCount > capabilities.maxImageCount) {
		info.minImageCount = capabilities.maxImageCount;
	}

	if (capabilities.currentExtent.width == 0xffffffff) {
		info.imageExtent.width = width;
		info.imageExtent.height = height;
	} else {
		info.imageExtent.width = capabilities.currentExtent.width;
		info.imageExtent.height = capabilities.currentExtent.height;
	}

	m_swapchainExtent = info.imageExtent;

	result = m_device.createSwapchainKHR(&info, nullptr, &m_swapchain);
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to create swapchain. Error code: %d", result);
		return false;
	}

	result = m_device.getSwapchainImagesKHR(m_swapchain, &m_frameCount, nullptr);
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to get swapchain image count. Error code: %d", result);
		return false;
	}

	m_frameData = new FrameData[m_frameCount];

	auto resultValue = m_device.getSwapchainImagesKHR(m_swapchain);
	if (resultValue.result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to get swapchain images. Error code: %d", resultValue.result);
		return false;
	}
	for (uint32_t i = 0; i < m_frameCount; ++i) {
		m_frameData[i].backbuffer = resultValue.value[i];
	}

	if (oldSwapchain) {
		m_device.destroySwapchainKHR(oldSwapchain, nullptr);
	}

	return true;
}

bool Visualizer::RenderImGui(vk::Semaphore *imageAcquiredSemaphore) {
	vk::Result result;

	FrameData *frame = &m_frameData[m_frameIndex];
	{
		result = m_device.waitForFences(1, &frame->fence, VK_TRUE, UINT64_MAX);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to wait for fence. Error code: %d", result);
			return false;
		}

		result = m_device.resetFences(1, &frame->fence);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to wait for fence. Error code: %d", result);
			return false;
		}
	}
	{
		result = m_device.resetCommandPool(frame->commandPool, {});
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to reset command pool. Error code: %d", result);
			return false;
		}

		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		result = frame->commandBuffer.begin(&beginInfo);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to begin command buffer. Error code: %d", result);
			return false;
		}
	}
	{
		vk::RenderPassBeginInfo beginInfo;
		beginInfo.renderPass = m_renderPass;
		beginInfo.framebuffer = frame->frameBuffer;
		beginInfo.renderArea.extent = m_swapchainExtent;
		beginInfo.clearValueCount = 0;
		beginInfo.pClearValues = nullptr;

		frame->commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
	}

	// Record Imgui Draw Data and draw funcs into command buffer
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), (VkCommandBuffer)frame->commandBuffer);

	// Submit command buffer
	frame->commandBuffer.endRenderPass();
	{
		vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::SubmitInfo info;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = imageAcquiredSemaphore;
		info.pWaitDstStageMask = &waitStage;
		info.commandBufferCount = 1;
		info.pCommandBuffers = &frame->commandBuffer;
		info.signalSemaphoreCount = 1;
		info.pSignalSemaphores = &frame->imguiRenderCompleted;

		result = frame->commandBuffer.end();
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to end command buffer. Error code: %d", result);
			return false;
		}

		m_graphicsQueue.submit(1, &info, frame->fence);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to submit command buffer. Error code: %d", result);
			return false;
		}
	}

	return true;
}

} // End of namespace Lantern
