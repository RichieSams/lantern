/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "common/global_args.h"

#include "graphics/atomic_frame_buffer.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl.h>

#include <cstdio>
#include <cstdlib>


namespace Lantern {

Visualizer::Visualizer(GlobalArgs *globalArgs)
		: m_globalArgs(globalArgs) {
	m_tempFrameBuffer = new float3[globalArgs->FrameBuffer->Width() * globalArgs->FrameBuffer->Height()];
}

Visualizer::~Visualizer() {
	delete[] m_tempFrameBuffer;
}

static void error_callback(int error, const char *description) {
	printf("Error %d: %s\n", error, description);
}

void Visualizer::Run() {
	Init();

	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(m_window)) {
		glfwWaitEvents();
		m_imGuiImpl.NewFrame();

		if (m_globalArgs->RenderChanged.load()) {
			CopyFrameBufferToGPU();
			m_globalArgs->RenderChanged.store(false);
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

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(m_globalArgs->FrameBuffer->Width(), m_globalArgs->FrameBuffer->Height(), "Lantern", nullptr, nullptr);
	m_window = window;

	// Setup ImGui binding
	m_imGuiImpl.InitImpl(window);

	glfwMakeContextCurrent(window);

	glfwGetFramebufferSize(window, &m_clientWidth, &m_clientHeight);
	glViewport(0, 0, m_clientWidth, m_clientHeight);

	// Create a texture 
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_clientWidth, m_clientHeight, 0, GL_RGB, GL_FLOAT, m_tempFrameBuffer);

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
	uint width = m_globalArgs->FrameBuffer->Width();
	uint height = m_globalArgs->FrameBuffer->Height();

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			m_globalArgs->FrameBuffer->GetPixel(x, y, m_tempFrameBuffer[y * width + x]);
		}
	}

	// Update Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, m_clientWidth, m_clientHeight, GL_RGB, GL_FLOAT, m_tempFrameBuffer);
}


} // End of namespace Lantern
