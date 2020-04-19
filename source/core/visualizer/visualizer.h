/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "math/int_types.h"

#include "camera/frame_buffer.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_vulkan.h"

#define VULKAN_HPP_NO_EXCEPTIONS
#include "vulkan/vulkan.hpp"

#include "vk_mem_alloc.h"

#include <atomic>


struct GLFWwindow;

namespace Lantern {

class Scene;

class Visualizer {
public:
	Visualizer(Scene *scene, FrameBuffer *currentFrameBuffer, std::atomic<FrameBuffer *> *swapFrameBuffer);
	~Visualizer();

private:
	Scene *m_scene;

	FrameBuffer *m_currentFrameBuffer;
	std::atomic<FrameBuffer *> *m_swapFrameBuffer;

	FrameBuffer m_accumulationFrameBuffer;

	GLFWwindow *m_window;

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

	uint32_t m_frameCount;
	struct FrameData {
		vk::CommandPool commandPool;
		vk::CommandBuffer commandBuffer;

		vk::Fence submitFinished;

		vk::Semaphore imageAcquired;
		vk::Semaphore imguiRenderCompleted;

		vk::Image backbuffer;
		vk::ImageView backbufferView;
		vk::Framebuffer frameBuffer;

		vk::Image stagingImage;
		vk::ImageView stagingImageView;
		VmaAllocation stagingBufferAllocation;
		VmaAllocationInfo stagingBufferAllocInfo;

		vk::DescriptorSet descriptorSet;
	};
	FrameData *m_frameData;

	uint32_t m_frameIndex;

	vk::ShaderModule m_vertexShader;
	vk::ShaderModule m_pixelShader;

	vk::Sampler m_sampler;
	vk::DescriptorSetLayout m_descriptorSetLayout;

	vk::PipelineLayout m_mainPipelineLayout;
	vk::Pipeline m_mainPipeline;

	vk::RenderPass m_mainRenderPass;
	vk::RenderPass m_imguiRenderPass;

	// GUI variables
	float m_frameTime[32];
	size_t m_frameTimeBin;
	float m_frameTimeSum;

	int m_selectedToneMapper;
	float m_exposure;

public:
	bool Init(int width, int height);
	void Run();
	void Shutdown();

	static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void CharCallback(GLFWwindow *window, unsigned int c);

private:
	bool RenderFrame();

	bool RenderImage(FrameData *frame);
	bool RenderImGui(FrameData *frame);

	bool InitVulkan();
	bool InitVulkanWindow(int width, int height);
	bool CreateSwapChain(int width, int height);
};

} // End of namespace Lantern
