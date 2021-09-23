/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#define NOMINMAX
#include "visualizer/visualizer.h"

#include "visualizer/shaders/final_resolve_ps.spv.h"
#include "visualizer/shaders/fullscreen_triangle_vs.spv.h"

#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include "GLFW/glfw3.h"

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <random>

namespace Lantern {

static const char *const kValidationLayers[] = {
    "VK_LAYER_LUNARG_standard_validation"};

static const char *const kDeviceExtensions[] = {
    "VK_KHR_swapchain"};

template <typename T, std::size_t N>
inline std::size_t SizeOfArray(const T (&)[N]) {
	return N;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
    VkDebugReportFlagsEXT flags,
    VkDebugReportObjectTypeEXT objType,
    uint64_t obj,
    size_t location,
    int32_t code,
    const char *layerPrefix,
    const char *msg,
    void *userData) {

	printf("Validation layer: %s\n", msg);

	return VK_FALSE;
}

// Needed for message pump callbacks
Visualizer *g_visualizer;

Visualizer::Visualizer()
        : m_window(nullptr) {
	g_visualizer = this;
}

Visualizer::~Visualizer() {
}

static void ErrorCallback(int error, const char *description) {
	printf("Error %d: %s\n", error, description);
}

void Visualizer::Run() {
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

void Visualizer::CharCallback(GLFWwindow *window, unsigned c) {
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
	ImGui_ImplVulkan_Init(&initInfo, (VkRenderPass)m_imguiRenderPass);

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

	// Set up GUI variables
	memset(m_frameTime, 0, sizeof(m_frameTime[0]) * SizeOfArray(m_frameTime));
	m_frameTimeSum = 0.0f;
	m_frameTimeBin = 0;

	m_selectedToneMapper = 1;
	m_exposure = 0.0f;

	return true;
}

void Visualizer::Shutdown() {
	m_device.waitIdle();

	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	m_device.destroyDescriptorSetLayout(m_descriptorSetLayout, nullptr);
	m_device.destroySampler(m_sampler, nullptr);

	m_device.destroyPipeline(m_mainPipeline, nullptr);
	m_device.destroyPipelineLayout(m_mainPipelineLayout, nullptr);

	m_device.destroyShaderModule(m_pixelShader, nullptr);
	m_device.destroyShaderModule(m_vertexShader, nullptr);

	m_device.destroyRenderPass(m_imguiRenderPass, nullptr);
	m_device.destroyRenderPass(m_mainRenderPass, nullptr);

	for (uint32_t i = 0; i < m_frameCount; ++i) {
		FrameData *data = &m_frameData[i];

		m_device.destroyFramebuffer(data->frameBuffer, nullptr);
		m_device.destroyImageView(data->backbufferView, nullptr);
		m_device.destroyFence(data->submitFinished, nullptr);
		m_device.destroySemaphore(data->imageAcquired, nullptr);
		m_device.destroySemaphore(data->imguiRenderCompleted, nullptr);
		m_device.destroyCommandPool(data->commandPool, nullptr); // This will clean up the associated command buffers

		m_device.destroyImageView(data->stagingImageView, nullptr);
		vmaDestroyImage(m_allocator, data->stagingImage, data->stagingBufferAllocation);
	}
	delete[] m_frameData;

	vmaDestroyAllocator(m_allocator);

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
	auto start = std::chrono::high_resolution_clock::now();

	// Start the Dear ImGui frame
	ImGui_ImplVulkan_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::Begin("Visualizer Stats", nullptr, ImVec2(0, 0), -1, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		const float frameTime = m_frameTimeSum / SizeOfArray(m_frameTime);
		const float fps = 1000.0f / frameTime;

		ImGui::Text("%.1f ms/frame (%.0f FPS)", frameTime, fps);
	}
	ImGui::End();

	// Rendering

	// Accumulate pixels generated by the Renderer

	// Acquire the next image to render to
	// We always use the semaphore of the "last" index, since there's no way to know the current index without supplying a semaphore
	vk::Semaphore imageAcquiredSemaphore = m_frameData[m_frameIndex].imageAcquired;

	vk::Result result = m_device.acquireNextImageKHR(m_swapchain, UINT64_MAX, imageAcquiredSemaphore, vk::Fence(nullptr), &m_frameIndex);
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to acquire next image. Error code: %d", result);
		return false;
	}

	FrameData *frame = &m_frameData[m_frameIndex];

	// Wait for the frame data to be free, then clear it
	result = m_device.waitForFences(1, &frame->submitFinished, VK_TRUE, UINT64_MAX);
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to wait for fence. Error code: %d", result);
		return false;
	}

	result = m_device.resetFences(1, &frame->submitFinished);
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to wait for fence. Error code: %d", result);
		return false;
	}
	result = m_device.resetCommandPool(frame->commandPool, {});
	if (result != vk::Result::eSuccess) {
		printf("Vulkan: Failed to reset command pool. Error code: %d", result);
		return false;
	}

	// Copy Renderer data to the GPU
	//float *mappedData = (float *)frame->stagingBufferAllocInfo.pMappedData;
	//for (uint j = 0; j < m_accumulationFrameBuffer.Height; ++j) {
	//	const size_t offset = j * m_accumulationFrameBuffer.Width;
	//	for (uint i = 0; i < m_accumulationFrameBuffer.Width; ++i) {
	//		const size_t frameBufferIndex = offset + i;
	//		const size_t mappedDataIndex = frameBufferIndex * 4;

	//		float3 &color = m_accumulationFrameBuffer.ColorData[frameBufferIndex];
	//		uint sampleCount = m_accumulationFrameBuffer.ColorSampleCount[frameBufferIndex];
	//		mappedData[mappedDataIndex + 0] = color.r / float(sampleCount); // Red
	//		mappedData[mappedDataIndex + 1] = color.g / float(sampleCount); // Green
	//		mappedData[mappedDataIndex + 2] = color.b / float(sampleCount); // Blue
	//		mappedData[mappedDataIndex + 3] = 1.0f;                         // Alpha

	//		minSPP = std::min(minSPP, sampleCount);
	//		maxSPP = std::max(maxSPP, sampleCount);
	//		sumSPP += sampleCount;
	//	}
	//}

	{
		// Flush to GPU
		vk::MappedMemoryRange flushRange;
		flushRange.memory = (vk::DeviceMemory)frame->stagingBufferAllocInfo.deviceMemory;
		flushRange.offset = 0;
		flushRange.size = vk::DeviceSize(VK_WHOLE_SIZE);

		if (m_device.flushMappedMemoryRanges(flushRange) != vk::Result::eSuccess) {
			printf("Vulkan: Failed to flush staging buffer");
			return false;
		}
	}

	ImGui::SetNextWindowPos(ImVec2(0, 50));
	ImGui::Begin("Integrator Stats", nullptr, ImVec2(0, 0), -1, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse);
	{
		//ImGui::Text("%u Min Samples Per Pixel", minSPP);
		//ImGui::Text("%u Max Samples Per Pixel", maxSPP);
		//ImGui::Text("%u Avg Samples Per Pixel", (uint)(sumSPP / (m_accumulationFrameBuffer.Width * m_accumulationFrameBuffer.Height)));
	}
	ImGui::End();

	{
		vk::CommandBufferBeginInfo beginInfo;
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		result = frame->commandBuffer.begin(&beginInfo);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to begin command buffer. Error code: %d", result);
			return false;
		}
	}

	RenderImage(frame);

	// End the ImGUI frame and record the commands to the command buffer
	ImGui::Render();
	RenderImGui(frame);

	// End and submit command buffer
	{
		vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;

		vk::SubmitInfo info;
		info.waitSemaphoreCount = 1;
		info.pWaitSemaphores = &imageAcquiredSemaphore;
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

		m_graphicsQueue.submit(1, &info, frame->submitFinished);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to submit command buffer. Error code: %d", result);
			return false;
		}
	}

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

	auto end = std::chrono::high_resolution_clock::now();
	float diff = std::chrono::duration<float, std::milli>(end - start).count();

	size_t index = m_frameTimeBin & (SizeOfArray(m_frameTime) - 1);
	m_frameTimeSum -= m_frameTime[index];
	m_frameTime[index] = diff;
	m_frameTimeSum += diff;
	++m_frameTimeBin;

	return true;
}

static vk::SurfaceFormatKHR GetSurfaceFormat(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, const vk::Format *requestedFormats, size_t requestedFormatsCount, const vk::ColorSpaceKHR requestedColorSpace) {
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
		unsigned glfwExtensionCount = 0;
		const char **glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

		std::vector<const char *> extensions;
		for (unsigned i = 0; i < glfwExtensionCount; ++i) {
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
		instanceCreateInfo.enabledExtensionCount = (uint32_t)extensions.size();
		instanceCreateInfo.ppEnabledExtensionNames = &extensions[0];
		instanceCreateInfo.enabledLayerCount = (uint32_t)SizeOfArray(kValidationLayers);
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
		for (uint32_t i = 0; i < (uint32_t)queueFamilyProperties.size(); ++i) {
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
		const float queuePriority[] = {1.0f};

		vk::DeviceQueueCreateInfo queueCreateInfo;
		queueCreateInfo.queueFamilyIndex = m_graphicsQueueFamilyIndex;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = queuePriority;

		vk::DeviceCreateInfo deviceCreateInfo;
		deviceCreateInfo.queueCreateInfoCount = 1;
		deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
		deviceCreateInfo.enabledExtensionCount = (uint32_t)SizeOfArray(kDeviceExtensions);
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
		createInfo.maxSets = 1000 * (uint32_t)SizeOfArray(poolSizes);
		createInfo.poolSizeCount = (uint32_t)SizeOfArray(poolSizes);
		createInfo.pPoolSizes = poolSizes;

		result = m_device.createDescriptorPool(&createInfo, nullptr, &m_descriptorPool);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create descriptor pool. Error code: %d", result);
			return false;
		}
	}

	return true;
}

struct PushConstants {
	int SelectedToneMapper;
	float Exposure;
};

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

	// Create the memory allocator
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.physicalDevice = static_cast<VkPhysicalDevice>(m_physicalDevice);
	allocatorCreateInfo.device = static_cast<VkDevice>(m_device);

	vmaCreateAllocator(&allocatorCreateInfo, &m_allocator);

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

				result = m_device.createFence(&info, nullptr, &m_frameData[i].submitFinished);
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

	// Create the main Render Pass
	{
		vk::AttachmentDescription attachment;
		attachment.format = m_surfaceFormat.format;
		attachment.samples = vk::SampleCountFlagBits::e1;
		attachment.loadOp = vk::AttachmentLoadOp::eDontCare;
		attachment.storeOp = vk::AttachmentStoreOp::eStore;
		attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		attachment.initialLayout = vk::ImageLayout::eUndefined;
		attachment.finalLayout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::AttachmentReference colorAttachment;
		colorAttachment.attachment = 0;
		colorAttachment.layout = vk::ImageLayout::eColorAttachmentOptimal;

		vk::SubpassDescription subpass;
		subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachment;

		vk::SubpassDependency dependencies[2];
		// External to image copy
		dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[0].dstSubpass = 0;
		dependencies[0].srcStageMask = vk::PipelineStageFlagBits::eBottomOfPipe;
		dependencies[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependencies[0].srcAccessMask = vk::AccessFlags();
		dependencies[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		dependencies[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;
		// Image copy to external
		dependencies[1].srcSubpass = 0;
		dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		dependencies[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependencies[1].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		dependencies[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		dependencies[1].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		dependencies[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;

		vk::RenderPassCreateInfo info;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 2;
		info.pDependencies = dependencies;

		result = m_device.createRenderPass(&info, nullptr, &m_mainRenderPass);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create render pass. Error code: %d", result);
			return false;
		}
	}

	// Create the ImGUI Render Pass
	{
		vk::AttachmentDescription attachment;
		attachment.format = m_surfaceFormat.format;
		attachment.samples = vk::SampleCountFlagBits::e1;
		attachment.loadOp = vk::AttachmentLoadOp::eLoad;
		attachment.storeOp = vk::AttachmentStoreOp::eStore;
		attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
		attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
		attachment.initialLayout = vk::ImageLayout::eColorAttachmentOptimal;
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
		dependency.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
		dependency.dependencyFlags = vk::DependencyFlagBits::eByRegion;

		vk::RenderPassCreateInfo info;
		info.attachmentCount = 1;
		info.pAttachments = &attachment;
		info.subpassCount = 1;
		info.pSubpasses = &subpass;
		info.dependencyCount = 1;
		info.pDependencies = &dependency;

		result = m_device.createRenderPass(&info, nullptr, &m_imguiRenderPass);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create render pass. Error code: %d", result);
			return false;
		}
	}

	// Load the shaders
	{
		vk::ShaderModuleCreateInfo info;
		info.codeSize = fullscreen_triangle_vs_spv_len;
		info.pCode = (uint32_t *)fullscreen_triangle_vs_spv;

		result = m_device.createShaderModule(&info, nullptr, &m_vertexShader);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to load vertex shader. Error code: %d", result);
			return false;
		}
	}
	{
		vk::ShaderModuleCreateInfo info;
		info.codeSize = final_resolve_ps_spv_len;
		info.pCode = (uint32_t *)final_resolve_ps_spv;

		result = m_device.createShaderModule(&info, nullptr, &m_pixelShader);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to load vertex shader. Error code: %d", result);
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

	// Create framebuffers
	{
		vk::FramebufferCreateInfo info;
		info.renderPass = m_mainRenderPass;
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

	// Create texture sampler
	{
		vk::SamplerCreateInfo info;
		info.magFilter = vk::Filter::eLinear;
		info.minFilter = vk::Filter::eLinear;
		info.addressModeU = vk::SamplerAddressMode::eRepeat;
		info.addressModeV = vk::SamplerAddressMode::eRepeat;
		info.addressModeW = vk::SamplerAddressMode::eRepeat;
		info.anisotropyEnable = VK_FALSE;
		info.unnormalizedCoordinates = VK_FALSE;
		info.compareEnable = VK_FALSE;
		info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		info.mipLodBias = 0.0f;
		info.minLod = 0.0f;
		info.maxLod = 0.0f;

		result = m_device.createSampler(&info, nullptr, &m_sampler);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create texture sampler. Error code: %d", result);
			return false;
		}
	}

	// Create descriptor set layout
	{
		vk::DescriptorSetLayoutBinding binding;
		binding.binding = 0;
		binding.descriptorCount = 1;
		binding.descriptorType = vk::DescriptorType::eCombinedImageSampler;
		binding.pImmutableSamplers = nullptr;
		binding.stageFlags = vk::ShaderStageFlagBits::eFragment;

		vk::DescriptorSetLayoutCreateInfo info;
		info.bindingCount = 1;
		info.pBindings = &binding;

		result = m_device.createDescriptorSetLayout(&info, nullptr, &m_descriptorSetLayout);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create descriptor set layout. Error code: %d", result);
			return false;
		}
	}

	// Create descriptor sets
	std::vector<vk::DescriptorSet> descriptorSets(m_frameCount);
	{
		std::vector<vk::DescriptorSetLayout> layouts(m_frameCount, m_descriptorSetLayout);

		vk::DescriptorSetAllocateInfo allocInfo;
		allocInfo.descriptorPool = m_descriptorPool;
		allocInfo.descriptorSetCount = m_frameCount;
		allocInfo.pSetLayouts = layouts.data();

		result = m_device.allocateDescriptorSets(&allocInfo, descriptorSets.data());
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create descriptor sets. Error code: %d", result);
			return false;
		}
	}

	// Create staging images
	{
		vk::ImageCreateInfo imageInfo;
		imageInfo.imageType = vk::ImageType::e2D;
		imageInfo.format = vk::Format::eR32G32B32A32Sfloat;
		imageInfo.extent = vk::Extent3D(m_swapchainExtent.width, m_swapchainExtent.height, 1);
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = vk::SampleCountFlagBits::e1;
		imageInfo.tiling = vk::ImageTiling::eLinear;
		imageInfo.usage = vk::ImageUsageFlagBits::eSampled;
		imageInfo.sharingMode = vk::SharingMode::eExclusive;
		imageInfo.initialLayout = vk::ImageLayout::eUndefined;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
		allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;

		for (unsigned i = 0; i < m_frameCount; ++i) {
			FrameData *frame = &m_frameData[i];

			if (vmaCreateImage(m_allocator, (VkImageCreateInfo *)&imageInfo, &allocInfo, (VkImage *)&frame->stagingImage, &frame->stagingBufferAllocation, &frame->stagingBufferAllocInfo) != VK_SUCCESS) {
				printf("Vulkan: Failed to create staging image");
				return false;
			}

			// Transition image to Shader Read Only Optimal
			{
				vk::CommandBufferBeginInfo beginInfo;
				beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

				result = frame->commandBuffer.begin(&beginInfo);
				if (result != vk::Result::eSuccess) {
					printf("Vulkan: Failed to begin command buffer. Error code: %d", result);
					return false;
				}

				vk::ImageMemoryBarrier barrier;
				barrier.srcAccessMask = (vk::AccessFlagBits)0;
				barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
				barrier.oldLayout = vk::ImageLayout::eUndefined;
				barrier.newLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.image = frame->stagingImage;
				barrier.subresourceRange = vk::ImageSubresourceRange(
				    vk::ImageAspectFlagBits::eColor, // aspectMask
				    0,                               // baseMipLevel
				    1,                               // levelCount
				    0,                               // baseArrayLayer
				    1                                // layerCount
				);

				frame->commandBuffer.pipelineBarrier(
				    vk::PipelineStageFlagBits::eTopOfPipe,      // srcStageMask
				    vk::PipelineStageFlagBits::eFragmentShader, // dstStageMask
				    vk::DependencyFlagBits(),                   // dependencyFlags
				    0,                                          // memoryBarrierCount
				    nullptr,                                    // pMemoryBarriers
				    0,                                          // bufferMemoryBarrierCount
				    nullptr,                                    // pBufferMemoryBarriers
				    1,                                          // imageMemoryBarrierCount
				    &barrier                                    // pImageMemoryBarriers
				);

				frame->commandBuffer.end();

				vk::SubmitInfo submitInfo;
				submitInfo.commandBufferCount = 1;
				submitInfo.pCommandBuffers = &frame->commandBuffer;

				m_graphicsQueue.submit(1, &submitInfo, vk::Fence());
				m_graphicsQueue.waitIdle();
			}

			// Create image view
			{
				vk::ImageViewCreateInfo info;
				info.image = frame->stagingImage;
				info.viewType = vk::ImageViewType::e2D;
				info.format = vk::Format::eR32G32B32A32Sfloat;
				info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
				info.subresourceRange.baseMipLevel = 0;
				info.subresourceRange.levelCount = 1;
				info.subresourceRange.baseArrayLayer = 0;
				info.subresourceRange.layerCount = 1;

				result = m_device.createImageView(&info, nullptr, &frame->stagingImageView);
				if (result != vk::Result::eSuccess) {
					printf("Vulkan: Failed to create staging buffer image view. Error code: %d", result);
					return false;
				}
			}

			// Update descriptor sets
			{
				frame->descriptorSet = descriptorSets[i];

				vk::DescriptorImageInfo info;
				info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
				info.imageView = frame->stagingImageView;
				info.sampler = m_sampler;

				vk::WriteDescriptorSet descriptorWrites[1];
				descriptorWrites[0].dstSet = frame->descriptorSet;
				descriptorWrites[0].dstBinding = 0;
				descriptorWrites[0].dstArrayElement = 0;
				descriptorWrites[0].descriptorType = vk::DescriptorType::eCombinedImageSampler;
				descriptorWrites[0].descriptorCount = 1;
				descriptorWrites[0].pImageInfo = &info;

				m_device.updateDescriptorSets((uint32_t)SizeOfArray(descriptorWrites), descriptorWrites, 0, nullptr);
			}
		}
	}

	// Create the main graphics pipeline
	{
		vk::PipelineVertexInputStateCreateInfo vertexInput;
		vertexInput.vertexBindingDescriptionCount = 0;
		vertexInput.pVertexBindingDescriptions = nullptr;
		vertexInput.vertexAttributeDescriptionCount = 0;
		vertexInput.pVertexAttributeDescriptions = nullptr;

		vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
		inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		vk::Viewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = (float)m_swapchainExtent.width;
		viewport.height = (float)m_swapchainExtent.height;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		vk::Rect2D scissor;
		scissor.offset = vk::Offset2D(0, 0);
		scissor.extent = m_swapchainExtent;

		vk::PipelineViewportStateCreateInfo viewportState;
		viewportState.viewportCount = 1;
		viewportState.pViewports = &viewport;
		viewportState.scissorCount = 1;
		viewportState.pScissors = &scissor;

		vk::PipelineRasterizationStateCreateInfo rasterizer;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = vk::PolygonMode::eFill;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = vk::CullModeFlagBits::eBack;
		rasterizer.frontFace = vk::FrontFace::eClockwise;

		vk::PipelineMultisampleStateCreateInfo multisample;
		multisample.sampleShadingEnable = VK_FALSE;
		multisample.rasterizationSamples = vk::SampleCountFlagBits::e1;
		multisample.minSampleShading = 1.0f;
		multisample.pSampleMask = nullptr;
		multisample.alphaToCoverageEnable = VK_FALSE;

		vk::PipelineColorBlendAttachmentState colorBlendAttachment;
		colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
		colorBlendAttachment.blendEnable = VK_FALSE;

		vk::PipelineColorBlendStateCreateInfo colorBlending;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = vk::LogicOp::eCopy;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;

		vk::PushConstantRange pushConstantRange;
		pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eFragment;
		pushConstantRange.offset = 0;
		pushConstantRange.size = sizeof(PushConstants);

		vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
		pipelineLayoutInfo.setLayoutCount = 1;
		pipelineLayoutInfo.pSetLayouts = &m_descriptorSetLayout;
		pipelineLayoutInfo.pushConstantRangeCount = 1;
		pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

		result = m_device.createPipelineLayout(&pipelineLayoutInfo, nullptr, &m_mainPipelineLayout);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create main pipeline layout. Error code: %d", result);
			return false;
		}

		vk::PipelineShaderStageCreateInfo shaderStages[2];
		shaderStages[0].stage = vk::ShaderStageFlagBits::eVertex;
		shaderStages[0].module = m_vertexShader;
		shaderStages[0].pName = "main";
		shaderStages[1].stage = vk::ShaderStageFlagBits::eFragment;
		shaderStages[1].module = m_pixelShader;
		shaderStages[1].pName = "main";

		vk::GraphicsPipelineCreateInfo pipelineInfo;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInput;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisample;
		pipelineInfo.pDepthStencilState = nullptr;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = nullptr;
		pipelineInfo.layout = m_mainPipelineLayout;
		pipelineInfo.renderPass = m_mainRenderPass;
		pipelineInfo.subpass = 0;
		pipelineInfo.basePipelineHandle = vk::Pipeline();
		pipelineInfo.basePipelineIndex = -1;

		result = m_device.createGraphicsPipelines(vk::PipelineCache(), 1, &pipelineInfo, nullptr, &m_mainPipeline);
		if (result != vk::Result::eSuccess) {
			printf("Vulkan: Failed to create main pipeline. Error code: %d", result);
			return false;
		}
	}

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
	info.imageUsage = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferDst;
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

	result = m_device.getSwapchainImagesKHR(m_swapchain, &m_frameCount, (vk::Image *)nullptr);
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

bool Visualizer::RenderImage(FrameData *frame) {
	{
		vk::RenderPassBeginInfo beginInfo;
		beginInfo.renderPass = m_mainRenderPass;
		beginInfo.framebuffer = frame->frameBuffer;
		beginInfo.renderArea.extent = m_swapchainExtent;
		beginInfo.clearValueCount = 0;
		beginInfo.pClearValues = nullptr;

		frame->commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
	}

	ImGui::Begin("Tonemapping", nullptr, ImVec2(0, 0), -1, ImGuiWindowFlags_AlwaysAutoResize);
	{
		ImGui::Combo("Tonemapper", &m_selectedToneMapper, "Clamp\0Filmic\0Gamma\0\0");
		ImGui::DragFloat("Exposure", &m_exposure, 0.1f, -10.0f, 10.0f, "%.1f");
	}
	ImGui::End();

	// Push tonemapping constants
	PushConstants constants;
	constants.SelectedToneMapper = m_selectedToneMapper;
	constants.Exposure = std::exp2f(m_exposure);
	frame->commandBuffer.pushConstants(m_mainPipelineLayout, vk::ShaderStageFlagBits::eFragment, 0, sizeof(PushConstants), &constants);

	// Draw a fullscreen triangle
	frame->commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_mainPipeline);
	frame->commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_mainPipelineLayout, 0, 1, &frame->descriptorSet, 0, nullptr);
	frame->commandBuffer.draw(3, 1, 0, 0);

	frame->commandBuffer.endRenderPass();

	return true;
}

bool Visualizer::RenderImGui(FrameData *frame) {
	{
		vk::RenderPassBeginInfo beginInfo;
		beginInfo.renderPass = m_imguiRenderPass;
		beginInfo.framebuffer = frame->frameBuffer;
		beginInfo.renderArea.extent = m_swapchainExtent;
		beginInfo.clearValueCount = 0;
		beginInfo.pClearValues = nullptr;

		frame->commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
	}

	// Record Imgui Draw Data and draw funcs into command buffer
	ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), (VkCommandBuffer)frame->commandBuffer);

	frame->commandBuffer.endRenderPass();

	return true;
}

} // End of namespace Lantern
