/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include <imgui_impl.h>


struct GLFWwindow;

namespace Lantern {

class Renderer;
class Scene;

class Visualizer {
public:
	Visualizer(Renderer *renderer, Scene *scene);
	~Visualizer();

private:
	Renderer *m_renderer;
	Scene *m_scene;
	float3 *m_tempFrameBuffer;

	GLFWwindow *m_window;
	double m_lastMousePosX;
	double m_lastMousePosY;
	bool m_leftMouseCaptured;
	bool m_middleMouseCaptured;

	ImGui::ImGuiImpl m_imGuiImpl;

public:
	void Run();

	static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void CursorPosCallback(GLFWwindow *window, double xpos, double ypos);
	static void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	static void CharCallback(GLFWwindow *window, unsigned int c);

private:
	void Init();
	void Shutdown();
	void CopyFrameBufferToGPU();
};

} // End of namespace Lantern
