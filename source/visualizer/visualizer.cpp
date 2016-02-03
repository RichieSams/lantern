/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl.h>

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <random>


namespace Lantern {

Visualizer::Visualizer(Renderer *renderer)
		: m_renderer(renderer),
		  m_window(nullptr) {
	m_tempFrameBuffer = new float3[renderer->GetFrameBuffer()->Width * renderer->GetFrameBuffer()->Height];
}

Visualizer::~Visualizer() {
	delete[] m_tempFrameBuffer;
}

static void error_callback(int error, const char *description) {
	printf("Error %d: %s\n", error, description);
}

void Visualizer::Run() {
	Init();

	auto lastRender = std::chrono::high_resolution_clock::now();
	
	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
		m_imGuiImpl.NewFrame();

		m_renderer->RenderFrame();

		auto currentTime = std::chrono::high_resolution_clock::now();
		int delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRender).count();
		if (delta >= 500) {
			CopyFrameBufferToGPU();
			lastRender = std::chrono::high_resolution_clock::now();
		}

		// Render scene
		glBegin(GL_QUADS);
			glTexCoord2d(0.0, 1.0);
			glVertex2d(-1.0, -1.0);

			glTexCoord2d(1.0, 1.0);
			glVertex2d(1.0, -1.0);

			glTexCoord2d(1.0, 0.0);
			glVertex2d(1.0, 1.0);

			glTexCoord2d(0.0, 0.0);
			glVertex2d(-1.0, 1.0);
		glEnd();

		// Render UI
		ImGui::Render();

		glfwSwapBuffers(m_window);
	}

	Shutdown();
}

void Visualizer::Init() {
	// Setup window
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	int width = m_renderer->GetFrameBuffer()->Width;
	int height = m_renderer->GetFrameBuffer()->Height;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(width, height, "Lantern", nullptr, nullptr);
	m_window = window;

	// Setup ImGui binding
	m_imGuiImpl.InitImpl(window);

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Create a texture 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, m_tempFrameBuffer);

	// Set up the texture
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);

	// Enable textures
	glEnable(GL_TEXTURE_2D);
}

void Visualizer::Shutdown() {
	glfwDestroyWindow(m_window);

	// Cleanup
	m_imGuiImpl.ShutdownImpl();
	glfwTerminate();
}


void Visualizer::CopyFrameBufferToGPU() {
	FrameBuffer *frameBuffer = m_renderer->GetFrameBuffer();

	uint width = frameBuffer->Width;
	uint height = frameBuffer->Height;

	// Update Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, frameBuffer->GetColorData());
}


} // End of namespace Lantern
