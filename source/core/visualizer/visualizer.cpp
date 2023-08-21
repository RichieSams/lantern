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
#include "imgui_internal.h"

#include "imgui_tex_inspect.h"
#include "tex_inspect_opengl.h"

#include "GLFW/glfw3.h"

#include "stb_image_write.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <random>
#include <stdio.h>
#include <stdlib.h>

#define OPENGL_DEBUG_PRINT 0

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

#if OPENGL_DEBUG_PRINT
static void MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar *message, const void *userParam) {
	printf("Error %s\n", message);
}
#endif

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

#if OPENGL_DEBUG_PRINT
	glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

	// Suggest we start maximized
	glfwWindowHint(GLFW_MAXIMIZED, 1);

	GLFWwindow *window = glfwCreateWindow(width, height, "Lantern", nullptr, nullptr);
	if (window == nullptr) {
		printf("Failed to create GLFW windows");
		return false;
	}
	m_window = window;

	glfwMakeContextCurrent(window);

	gl3wInit();

#if OPENGL_DEBUG_PRINT
	glDebugMessageCallback(MessageCallback, 0);
#endif

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
	ImGuiTexInspect::ImplOpenGL3_Init(glslVersion);

	// Initialize the TexInspect plugin
	ImGuiTexInspect::Init();
	ImGuiTexInspect::CreateContext();

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

	// Create render texture / copying buffers
	glGenTextures(1, &m_renderTexture);
	glBindTexture(GL_TEXTURE_2D, m_renderTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB32F, m_currentPresentationBuffer->Width, m_currentPresentationBuffer->Height, 0, GL_RGB, GL_FLOAT, nullptr);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenBuffers(2, m_renderPBOs);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_renderPBOs[0]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_currentPresentationBuffer->DataSize * sizeof(float), 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_renderPBOs[1]);
	glBufferData(GL_PIXEL_UNPACK_BUFFER, m_currentPresentationBuffer->DataSize * sizeof(float), 0, GL_STREAM_DRAW);
	glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

	return true;
}

void Visualizer::Shutdown() {
	glDeleteBuffers(2, m_renderPBOs);
	glDeleteTextures(1, &m_renderTexture);

	ImGuiTexInspect::DestroyContext(nullptr);
	ImGuiTexInspect::Shutdown();

	ImGuiTexInspect::ImplOpenGl3_Shutdown();
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
		const ImVec2 dockspaceSize = ImGui::GetContentRegionAvail();

		ImGuiID dockspaceID = ImGui::GetID("RootDock");
		ImGuiViewport *viewport = ImGui::GetMainViewport();
		ImGui::DockBuilderRemoveNode(dockspaceID);                       // Clear out existing layout
		ImGui::DockBuilderAddNode(dockspaceID, ImGuiDockNodeFlags_None); // Add empty node
		ImGui::DockBuilderSetNodeSize(dockspaceID, dockspaceSize);

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

	// Copy presentation buffer data to the GPU
	{
		m_currentFrameIndex = (m_currentFrameIndex + 1) % 2;
		unsigned int nextFrameIndex = (m_currentFrameIndex + 1) % 2;

		// bind the texture and PBO
		glBindTexture(GL_TEXTURE_2D, m_renderTexture);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_renderPBOs[m_currentFrameIndex]);

		// copy pixels from PBO to texture object
		// Use offset instead of pointer.
		glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_currentPresentationBuffer->Width, m_currentPresentationBuffer->Height, GL_RGB, GL_FLOAT, nullptr);

		// bind PBO to update texture source
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_renderPBOs[nextFrameIndex]);

		// Note that glMapBuffer() causes sync issue.
		// If GPU is working with this buffer, glMapBuffer() will wait(stall)
		// until GPU to finish its job. To avoid waiting (idle), you can call
		// first glBufferData() with NULL pointer before glMapBuffer().
		// If you do that, the previous data in PBO will be discarded and
		// glMapBuffer() returns a new allocated pointer immediately
		// even if GPU is still working with the previous data.
		glBufferData(GL_PIXEL_UNPACK_BUFFER, m_currentPresentationBuffer->DataSize * sizeof(float), 0, GL_STREAM_DRAW);

		// map the buffer object into client's memory
		GLubyte *ptr = (GLubyte *)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
		if (ptr) {
			memcpy(ptr, m_currentPresentationBuffer->ResolvedData, m_currentPresentationBuffer->DataSize * sizeof(float));
			glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER); // release the mapped buffer
		}

		// it is good idea to release PBOs with ID 0 after use.
		// Once bound with 0, all pixel operations are back to normal ways.
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	// Show the image in the ImGui Viewport
	if (ImGui::Begin("Viewport")) {
		ImVec2 maxSize = ImGui::GetContentRegionAvail();

		ImGuiTexInspect::BeginInspectorPanel(
		    "ViewportTextureInspector",
		    (ImTextureID)(uintptr_t)m_renderTexture,
		    ImVec2(m_currentPresentationBuffer->Width, m_currentPresentationBuffer->Height),
		    0,
		    ImGuiTexInspect::SizeIncludingBorder(maxSize));
		ImGuiTexInspect::EndInspectorPanel();
	}
	ImGui::End();

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
			char *buffer = (char *)malloc(m_currentPresentationBuffer->DataSize * 3 * sizeof(float));
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

} // End of namespace lantern
