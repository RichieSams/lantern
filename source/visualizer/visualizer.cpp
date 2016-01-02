/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "graphics/atomic_frame_buffer.h"

#include <GLFW/glfw3.h>

#include <cstdio>
#include <cstdlib>


namespace Lantern {

static void error_callback(int error, const char *description) {
	printf("Error %d: %s\n", error, description);
}

Visualizer::Visualizer(AtomicFrameBuffer *frameBuffer)
		: m_atomicFrameBuffer(frameBuffer) {
	m_tempFrameBuffer = new float3[frameBuffer->Width() * frameBuffer->Height()];
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

	GLFWwindow* window = glfwCreateWindow(m_atomicFrameBuffer->Width(), m_atomicFrameBuffer->Height(), "Lantern", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	int width, height;
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	glfwSwapInterval(1);
	while (!glfwWindowShouldClose(window)) {
		CopyAtomicFrameBufferToTemp();
		glDrawPixels(width, height, GL_RGB, GL_FLOAT, m_tempFrameBuffer);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glfwDestroyWindow(window);

	// Cleanup
	glfwTerminate();
}

void Visualizer::CopyAtomicFrameBufferToTemp() {
	uint width = m_atomicFrameBuffer->Width();
	uint height = m_atomicFrameBuffer->Height();

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			m_atomicFrameBuffer->GetPixel(x, y, m_tempFrameBuffer[y * width + x]);
		}
	}
}


} // End of namespace Lantern
