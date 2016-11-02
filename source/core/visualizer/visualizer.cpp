/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include "scene/scene.h"

#include <GLFW/glfw3.h>

#include <imgui.h>
#include <imgui_impl.h>

#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <random>


namespace Lantern {

// Needed for message pump callbacks
Visualizer *g_visualizer;

Visualizer::Visualizer(Renderer *renderer, Scene *scene)
		: m_renderer(renderer),
		  m_scene(scene),
		  m_window(nullptr),
		  m_leftMouseCaptured(false),
		  m_middleMouseCaptured(false) {
	m_tempFrameBuffer = new float3[scene->Camera->FrameBuffer.Width * scene->Camera->FrameBuffer.Height];
	g_visualizer = this;
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
	auto startTime = lastRender;

	glfwSwapInterval(0);
	while (!glfwWindowShouldClose(m_window)) {
		glfwPollEvents();
		m_imGuiImpl.NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 75));
		ImGui::Begin("Scene Options", nullptr, ImVec2(0, 0), -1.0f, ImGuiWindowFlags_AlwaysAutoResize);
		if (ImGui::Button("Reload Scene", ImVec2(125, 25))) {
			m_scene->ReloadSceneFromJSON();
			startTime = std::chrono::high_resolution_clock::now();
		}
		ImGui::End();

		m_renderer->RenderFrame();

		auto currentTime = std::chrono::high_resolution_clock::now();
		int delta = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastRender).count();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::Begin("Frame Stats", nullptr, ImVec2(0, 0), -1.0f, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("%d ms/frame (%.1f FPS)", delta, 1.0f / delta * 1000.0f);
		
		auto runTime = currentTime - startTime;
		int minutes = std::chrono::duration_cast<std::chrono::minutes>(runTime).count();
		int seconds = std::chrono::duration_cast<std::chrono::seconds>(runTime).count() - minutes * 60;
		ImGui::Text("Run time - %d:%d (min:sec)", minutes, seconds);
		ImGui::End();

		if (delta >= 0) {
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

void Visualizer::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS && mods == GLFW_MOD_ALT) {
		glfwGetCursorPos(window, &g_visualizer->m_lastMousePosX, &g_visualizer->m_lastMousePosY);
		g_visualizer->m_leftMouseCaptured = true;
	} else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
		g_visualizer->m_leftMouseCaptured = false;
	} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_PRESS && mods == GLFW_MOD_ALT) {
		glfwGetCursorPos(window, &g_visualizer->m_lastMousePosX, &g_visualizer->m_lastMousePosY);
		g_visualizer->m_middleMouseCaptured = true;
	} else if (button == GLFW_MOUSE_BUTTON_MIDDLE && action == GLFW_RELEASE) {
		g_visualizer->m_middleMouseCaptured = false;
	}

	g_visualizer->m_imGuiImpl.MouseButtonCallback(window, button, action, mods);
}

void Visualizer::CursorPosCallback(GLFWwindow *window, double xpos, double ypos) {
	if (g_visualizer->m_leftMouseCaptured) {
		double oldX = g_visualizer->m_lastMousePosX;
		double oldY = g_visualizer->m_lastMousePosY;
		glfwGetCursorPos(window, &g_visualizer->m_lastMousePosX, &g_visualizer->m_lastMousePosY);

		g_visualizer->m_scene->Camera->Rotate((float)(oldY - g_visualizer->m_lastMousePosY) / 300,
		                                     (float)(oldX - g_visualizer->m_lastMousePosX) / 300);

		g_visualizer->m_scene->Camera->FrameBuffer.Reset();
	} else if (g_visualizer->m_middleMouseCaptured) {
		double oldX = g_visualizer->m_lastMousePosX;
		double oldY = g_visualizer->m_lastMousePosY;
		glfwGetCursorPos(window, &g_visualizer->m_lastMousePosX, &g_visualizer->m_lastMousePosY);

		g_visualizer->m_scene->Camera->Pan((float)(oldX - g_visualizer->m_lastMousePosX) * 0.01,
		                                  (float)(g_visualizer->m_lastMousePosY - oldY) * 0.01);

		g_visualizer->m_scene->Camera->FrameBuffer.Reset();
	}
}

void Visualizer::ScrollCallback(GLFWwindow *window, double xoffset, double yoffset) {
	g_visualizer->m_scene->Camera->Zoom(yoffset);
	g_visualizer->m_scene->Camera->FrameBuffer.Reset();

	g_visualizer->m_imGuiImpl.ScrollCallback(window, xoffset, yoffset);
}

void Visualizer::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
	g_visualizer->m_imGuiImpl.KeyCallback(window, key, scancode, action, mods);
}

void Visualizer::CharCallback(GLFWwindow *window, uint c) {
	g_visualizer->m_imGuiImpl.CharCallback(window, c);
}

void Visualizer::Init() {
	// Setup window
	glfwSetErrorCallback(error_callback);
	if (!glfwInit()) {
		glfwTerminate();
		exit(EXIT_FAILURE);
	}

	int width = m_scene->Camera->FrameBuffer.Width;
	int height = m_scene->Camera->FrameBuffer.Height;

	glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
	GLFWwindow *window = glfwCreateWindow(width, height, "Lantern", nullptr, nullptr);
	m_window = window;

	// Setup ImGui binding
	m_imGuiImpl.InitImpl(window);

	// Bind callbacks
	glfwSetMouseButtonCallback(window, MouseButtonCallback);
	glfwSetCursorPosCallback(window, CursorPosCallback);
	glfwSetScrollCallback(window, ScrollCallback);
	glfwSetKeyCallback(window, KeyCallback);
	glfwSetCharCallback(window, CharCallback);

	// Set up the framebuffer
	glfwMakeContextCurrent(window);
	glfwGetFramebufferSize(window, &width, &height);
	glViewport(0, 0, width, height);

	// Create a texture
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_FLOAT, nullptr);
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
	FrameBuffer *frameBuffer = &m_scene->Camera->FrameBuffer;

	uint width = frameBuffer->Width;
	uint height = frameBuffer->Height;

	float3 *colorData = frameBuffer->GetColorData();
	float *weights = frameBuffer->GetWeights();

	for (uint i = 0; i < width * height; ++i) {
		m_tempFrameBuffer[i] = colorData[i] / weights[i];
	}

	// Update Texture
	glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB, GL_FLOAT, m_tempFrameBuffer);
}


} // End of namespace Lantern
