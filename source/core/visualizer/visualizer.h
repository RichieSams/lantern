/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"

#include "camera/frame_buffer.h"

#define VULKAN_HPP_NO_EXCEPTIONS
#include <vulkan/vulkan.hpp>

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
	double m_lastMousePosX;
	double m_lastMousePosY;
	bool m_leftMouseCaptured;
	bool m_middleMouseCaptured;

	vk::Instance m_instance;
	vk::DebugReportCallbackEXT m_debugCallback;

	vk::SurfaceKHR m_surface;

	vk::PhysicalDevice m_physicalDevice;
	vk::Device m_device;

	VmaAllocator m_allocator;
	
	vk::Queue m_graphicsQueue;
	vk::Queue m_presentQueue;
	vk::SwapchainKHR m_swapchain;
	vk::Format m_swapchainFormat;
	vk::Extent2D m_swapchainExtent;

	std::vector<vk::Image> m_swapChainImages;
	std::vector<vk::ImageView> m_swapChainImageViews;
	std::vector<vk::Framebuffer> m_frameBuffers;

	vk::ShaderModule m_vertexShader;
	vk::ShaderModule m_pixelShader;

	vk::PipelineLayout m_mainPipelineLayout;
	vk::Pipeline m_mainPipeline;

	vk::RenderPass m_renderPass;

	vk::CommandPool m_commandPool;
	std::vector<vk::CommandBuffer> m_commandBuffers;

	vk::Semaphore m_imageAvailable;
	vk::Semaphore m_renderFinished;

	std::vector<vk::Image> m_stagingImage;
	std::vector<VmaAllocation> m_stagingBufferAllocation;
	std::vector<VmaAllocationInfo> m_stagingBufferAllocInfo;

public:
	void Run();

	static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void CharCallback(GLFWwindow *window, unsigned int c);

private:
	bool Init();
	void Shutdown();

	bool RenderFrame();

	bool CreateSwapChain(uint width, uint height);
	void CleanUpSwapChainAndDependents();
	bool RecreateSwapChainAndDependents();

	bool CreateImageViews();
	bool CreateRenderPass();
	bool CreateGraphicsPipeline();
	bool CreateFrameBuffers();
	bool CreateCommandBuffers();
};

} // End of namespace Lantern
