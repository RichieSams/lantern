/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "common/global_args.h"

#include "graphics/atomic_frame_buffer.h"

#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>


namespace Lantern {

static void error_callback(int error, const char *description) {
	printf("Error %d: %s\n", error, description);
}

Visualizer::Visualizer(GlobalArgs *globalArgs)
		: m_globalArgs(globalArgs) {
	m_tempFrameBuffer = new float3[globalArgs->FrameBuffer->Width() * globalArgs->FrameBuffer->Height()];
}

Visualizer::~Visualizer() {
	delete[] m_tempFrameBuffer;
}

void Visualizer::Run() {
	// Setup window
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	GLFWwindow* window = glfwCreateWindow(m_globalArgs->FrameBuffer->Width(), m_globalArgs->FrameBuffer->Height(), "Lantern", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		if (m_globalArgs->RenderChanged.load()) {
			CopyFrameBufferToTemp();
			glDrawPixels(width, height, GL_RGB, GL_FLOAT, m_tempFrameBuffer);

			m_globalArgs->RenderChanged.store(false);
			
			glfwSwapBuffers(window);
		}
	}

	glfwDestroyWindow(window);

	// Cleanup
	glfwTerminate();
}

void Visualizer::CopyFrameBufferToTemp() {
	uint width = m_globalArgs->FrameBuffer->Width();
	uint height = m_globalArgs->FrameBuffer->Height();

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			m_globalArgs->FrameBuffer->GetPixel(x, y, m_tempFrameBuffer[y * width + x]);
		}
	}
}


} // End of namespace Lantern
