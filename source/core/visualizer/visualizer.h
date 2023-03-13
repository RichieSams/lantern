/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define VULKAN_HPP_NO_EXCEPTIONS
#include "vulkan/vulkan.hpp"

#include "vk_mem_alloc.h"

#include <atomic>
#include <chrono>

struct GLFWwindow;

namespace lantern {

class Scene;
struct PresentationBuffer;

class Visualizer {
public:
	Visualizer(PresentationBuffer *startingPresentationBuffer, std::atomic<uint64_t> *renderHostGenerationNumber, std::atomic<PresentationBuffer *> *swapPresentationBuffer);
	~Visualizer();

private:
	GLFWwindow *m_window;

	// Vulkan variables
	vk::Instance m_instance;
	vk::DebugReportCallbackEXT m_debugCallback;

	vk::SurfaceKHR m_surface;

	vk::PhysicalDevice m_physicalDevice;
	vk::Device m_device;

	VmaAllocator m_allocator;

	vk::DescriptorPool m_descriptorPool;

	vk::SurfaceFormatKHR m_surfaceFormat;
	vk::PresentModeKHR m_presentMode;

	uint32_t m_graphicsQueueFamilyIndex;
	vk::Queue m_graphicsQueue;
	vk::SwapchainKHR m_swapchain;
	vk::Extent2D m_swapchainExtent;

	uint32_t m_frameBufferCount;
	struct VulkanFrameData {
		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		vk::Fence submitFinished;

		vk::Semaphore imageAcquired;
		vk::Semaphore imguiRenderCompleted;

		vk::Image stagingImage;
		uint64_t stagingImagePitch;
		vk::ImageView stagingImageView;
		VmaAllocation stagingBufferAllocation;
		VmaAllocationInfo stagingBufferAllocInfo;

		vk::DescriptorSet descriptorSet;
	};
	VulkanFrameData *m_vulkanFrameData;

	struct VulkanBackBufferData {
		vk::Image backbuffer;
		vk::ImageView backbufferView;
		vk::Framebuffer mainFrameBuffer;
		vk::Framebuffer imguiFrameBuffer;
	};
	VulkanBackBufferData *m_vulkanBackBufferData;

	uint32_t m_currentFrameIndex;
	bool m_frameBufferResized;

	vk::ShaderModule m_vertexShader;
	vk::ShaderModule m_pixelShader;

	vk::Sampler m_sampler;
	vk::DescriptorSetLayout m_descriptorSetLayout;

	vk::PipelineLayout m_mainPipelineLayout;
	vk::Pipeline m_mainPipeline;

	vk::RenderPass m_mainRenderPass;
	vk::RenderPass m_imguiRenderPass;

	// Data from render host
	PresentationBuffer *m_currentPresentationBuffer;
	uint64_t m_presentationBufferGeneration;
	std::atomic<uint64_t> *m_renderHostGenerationNumber;
	std::atomic<PresentationBuffer *> *m_swapPresentationBuffer;

	// Render host GUI variables
	float m_renderTime[32];
	std::chrono::high_resolution_clock::time_point m_renderTimeStart;
	size_t m_renderTimeBin;

	// GUI variables
	float m_frameTime[32];
	size_t m_frameTimeBin;

	// Viewport variables
	float m_viewportZoom;

public:
	bool Init(int width, int height);
	void Run();
	void Shutdown();

private:
	bool RenderFrame();

	bool RenderImage(VulkanFrameData *frame, VulkanBackBufferData *backBuffer);
	bool RenderImGui(VulkanFrameData *frame, VulkanBackBufferData *backBuffer);

	bool InitVulkan();
	bool InitVulkanWindow();
	bool CreateSwapChain();
	bool CreateFrameBufferAndViews();
	bool RecreateSwapChain();
	void CleanupSwapChain();
};

} // End of namespace lantern
