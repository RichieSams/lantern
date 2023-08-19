/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#define NOMINMAX
#include "visualizer/visualizer.h"

#include "render_host/presentation_buffer.h"

#include "GL/gl3w.h"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "GLFW/glfw3.h"

#include "stb_image_write.h"

#include "imgui_internal.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <random>
#include <stdio.h>
#include <stdlib.h>

namespace lantern {

template <typename T, std::size_t N>
inline std::size_t SizeOfArray(const T (&)[N]) {
	return N;
}

// Needed for message pump callbacks
Visualizer *g_visualizer;

Visualizer::Visualizer(PresentationBuffer *startingPresentationBuffer, std::atomic<uint64_t> *renderHostGenerationNumber, std::atomic<PresentationBuffer *> *swapPresentationBuffer)
        : m_window(nullptr),
          m_currentPresentationBuffer(startingPresentationBuffer),
          m_presentationBufferGeneration(0),
          m_renderHostGenerationNumber(renderHostGenerationNumber),
          m_swapPresentationBuffer(swapPresentationBuffer) {
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
}

bool Visualizer::Init(int width, int height) {
	// Setup window
	glfwSetErrorCallback(ErrorCallback);
	if (!glfwInit()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

#if defined(__APPLE__)
	// GL 3.2 + GLSL 150
	const char *glslVersion = "#version 150";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac
#else
	// GL 3.0 + GLSL 130
	const char *glslVersion = "#version 130";
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

	GLFWwindow *window = glfwCreateWindow(width, height, "Lantern", nullptr, nullptr);
	if (window == nullptr) {
		printf("Failed to create GLFW windows");
		return false;
	}
	m_window = window;

	glfwMakeContextCurrent(window);

	gl3wInit();

	// Enable vsync
	glfwSwapInterval(1);

	// Setup ImGui binding
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGuiIO &io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard | ImGuiConfigFlags_DockingEnable;
	// io.ConfigWindowsMoveFromTitleBarOnly = true;
	io.ConfigWindowsResizeFromEdges = true;

	// Setup Platform/Renderer backends
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glslVersion);

	// Setup style
	ImGui::StyleColorsDark();

	// Set up GUI variables
	memset(m_renderTime, 0, sizeof(m_renderTime[0]) * SizeOfArray(m_renderTime));
	m_renderTimeBin = 0;
	m_renderTimeStart = std::chrono::high_resolution_clock::now();

	memset(m_frameTime, 0, sizeof(m_frameTime[0]) * SizeOfArray(m_frameTime));
	m_frameTimeBin = 0;

	// Set up the viewport variables
	m_viewportZoom = 1.0f;

	return true;
}

void Visualizer::Shutdown() {
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(m_window);
	glfwTerminate();
}

bool Visualizer::RenderFrame() {
	auto start = std::chrono::high_resolution_clock::now();

	// Start the Dear ImGui frame
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();

	// Create the "root" window
	{
		const ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(viewport->WorkSize);
		ImGui::SetNextWindowViewport(viewport->ID);

		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		windowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
		windowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("RootWindow", nullptr, windowFlags);
		ImGui::PopStyleVar(3);
	}

	// Now set up the dock

	// Only set the initial docking locations if someone hasn't already loaded them
	if (ImGui::DockBuilderGetNode(ImGui::GetID("RootDock")) == nullptr) {
		ImGuiID dockspaceID = ImGui::GetID("RootDock");
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::DockBuilderRemoveNode(dockspaceID);                            // Clear out existing layout
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_DockSpace); // Add empty node

		ImGuiID remaining;
		ImGuiID bottomDockID = ImGui::DockBuilderSplitNode(dockspaceID, ImGuiDir_Down, 0.20f, nullptr, &remaining);
		ImGuiID propertiesDockID = ImGui::DockBuilderSplitNode(remaining, ImGuiDir_Right, 0.20f, nullptr, &remaining);
		ImGuiID viewportDockID = remaining;

		ImGui::DockBuilderDockWindow("Viewport", viewportDockID);
		ImGui::DockBuilderDockWindow("Console", bottomDockID);
		ImGui::DockBuilderDockWindow("Properties", propertiesDockID);
		ImGui::DockBuilderFinish(dockspaceID);
	}

	// Create the docking
	ImGuiID dockspaceID = ImGui::GetID("RootDock");
	ImGui::DockSpace(dockspaceID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_None);

	// We can close the main window now
	// Since all other windows we create will be already inside that window (via the dock setup we just created), or floating
	ImGui::End();

	// Rendering

	// Get the latest image to present from the render host
	// Check if the generation number of the render host is newer
	const uint64_t newGenerationNumber = m_renderHostGenerationNumber->load(std::memory_order_acquire);
	if (newGenerationNumber > m_presentationBufferGeneration) {
		m_currentPresentationBuffer = std::atomic_exchange(m_swapPresentationBuffer, m_currentPresentationBuffer);

		const uint64_t numGenerations = newGenerationNumber - m_presentationBufferGeneration;
		m_presentationBufferGeneration = newGenerationNumber;

		auto end = std::chrono::high_resolution_clock::now();
		float diff = std::chrono::duration<float, std::milli>(end - m_renderTimeStart).count();
		m_renderTimeStart = end;

		size_t index = m_renderTimeBin & (SizeOfArray(m_renderTime) - 1);
		m_renderTime[index] = diff / (float)numGenerations;
		++m_renderTimeBin;
	}

	// // Copy presentation buffer data to the GPU
	// {
	// 	uint8_t *mappedData = (uint8_t *)frame->stagingBufferAllocInfo.pMappedData;
	// 	for (uint32_t y = 0; y < m_currentPresentationBuffer->Height; ++y) {
	// 		const size_t presentationOffset = y * m_currentPresentationBuffer->Width * 3;
	// 		const size_t stagingOffset = y * frame->stagingImagePitch;

	// 		memcpy(mappedData + stagingOffset, m_currentPresentationBuffer->ResolvedData + presentationOffset, m_currentPresentationBuffer->Width * sizeof(float) * 3);
	// 	}

	// 	// Flush to GPU
	// 	vk::MappedMemoryRange flushRange;
	// 	flushRange.memory = (vk::DeviceMemory)frame->stagingBufferAllocInfo.deviceMemory;
	// 	flushRange.offset = 0;
	// 	flushRange.size = vk::DeviceSize(VK_WHOLE_SIZE);

	// 	if (m_device.flushMappedMemoryRanges(flushRange) != vk::Result::eSuccess) {
	// 		printf("Vulkan: Failed to flush staging buffer");
	// 		return false;
	// 	}
	// }

	// {
	// 	vk::CommandBufferBeginInfo beginInfo;
	// 	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

	// 	result = frame->commandBuffer.begin(&beginInfo);
	// 	if (result != vk::Result::eSuccess) {
	// 		printf("Vulkan: Failed to begin command buffer. Error: %s", vk::to_string(result).c_str());
	// 		return false;
	// 	}
	// }

	// RenderImage(frame, backBuffer);

	// // Show the image in the ImGui Viewport
	// if (ImGui::Begin("Viewport")) {
	// 	ImVec2 min = ImGui::GetWindowContentRegionMin();
	// 	ImVec2 max = ImGui::GetWindowContentRegionMax();

	// 	float xRange = max.x - min.x;
	// 	float yRange = max.y - min.y;

	// 	// Keep the same aspect ratio
	// 	float imageScale = std::min(xRange / m_currentPresentationBuffer->Width, yRange / m_currentPresentationBuffer->Height);

	// 	ImGui::Image((ImTextureID)frame->descriptorSet, ImVec2(m_currentPresentationBuffer->Width * imageScale * m_viewportZoom, m_currentPresentationBuffer->Height * imageScale * m_viewportZoom));
	// }
	// ImGui::End();

	// Fill in the properties panel
	if (ImGui::Begin("Properties")) {
		ImGui::Text("Visualizer Stats");
		{
			float frameTimeSum = 0.0f;
			for (size_t i = 0; i < SizeOfArray(m_frameTime); ++i) {
				frameTimeSum += m_frameTime[i];
			}

			const float frameTime = frameTimeSum / SizeOfArray(m_frameTime);
			const float fps = 1000.0f / frameTime;

			ImGui::Text("%.1f ms/frame (%.0f FPS)", frameTime, fps);
		}
		ImGui::Separator();

		ImGui::Text("Integrator Stats");
		{
			float renderTimeSum = 0.0f;
			for (size_t i = 0; i < SizeOfArray(m_renderTime); ++i) {
				renderTimeSum += m_renderTime[i];
			}

			const float renderTime = renderTimeSum / SizeOfArray(m_renderTime);
			const float fps = 1000.0f / renderTime;

			ImGui::Text("%.1f ms/frame (%.0f FPS)", renderTime, fps);
		}
		ImGui::Separator();

		if (ImGui::Button("Save Image")) {
			char *buffer = (char *)malloc(m_currentPresentationBuffer->Width * m_currentPresentationBuffer->Height * 3);
			for (size_t y = 0; y < m_currentPresentationBuffer->Height; ++y) {
				size_t offset = y * m_currentPresentationBuffer->Width * 3;

				for (size_t x = 0; x < m_currentPresentationBuffer->Width * 3; ++x) {
					buffer[offset + x] = (char)(m_currentPresentationBuffer->ResolvedData[offset + x] * 255.0f);
				}
			}

			stbi_write_png("image.png", m_currentPresentationBuffer->Width, m_currentPresentationBuffer->Height, 3, buffer, m_currentPresentationBuffer->Width * 3);
			free(buffer);
		}
	}
	ImGui::End();

	if (ImGui::Begin("Console")) {
	}
	ImGui::End();

	// End the ImGUI frame and render it
	ImGui::Render();

	int displayW, displayH;
	glfwGetFramebufferSize(m_window, &displayW, &displayH);
	glViewport(0, 0, displayW, displayH);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

	glfwSwapBuffers(m_window);

	auto end = std::chrono::high_resolution_clock::now();
	float diff = std::chrono::duration<float, std::milli>(end - start).count();

	size_t index = m_frameTimeBin & (SizeOfArray(m_frameTime) - 1);
	m_frameTime[index] = diff;
	++m_frameTimeBin;

	return true;
}

// bool Visualizer::RenderImage(VulkanFrameData *frame, VulkanBackBufferData *backBuffer) {
// 	{
// 		vk::ClearValue clearValue(vk::ClearColorValue{std::array<float, 4>({0.846f, 0.933f, 0.949f, 1.0f})});

// 		vk::RenderPassBeginInfo beginInfo;
// 		beginInfo.renderPass = m_mainRenderPass;
// 		beginInfo.framebuffer = backBuffer->mainFrameBuffer;
// 		beginInfo.renderArea.extent = m_swapchainExtent;
// 		beginInfo.clearValueCount = 1;
// 		beginInfo.pClearValues = &clearValue;

// 		frame->commandBuffer.beginRenderPass(&beginInfo, vk::SubpassContents::eInline);
// 	}

// 	frame->commandBuffer.endRenderPass();

// 	return true;
// }

} // End of namespace lantern
