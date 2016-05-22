// ImGui GLFW binding with OpenGL
// You can copy and use unmodified imgui_impl_* files in your project. See main.cpp for an example of using this.
// If you use this binding you'll need to call 4 functions: ImGui_ImplXXXX_Init(), ImGui_ImplXXXX_NewFrame(), ImGui::Render() and ImGui_ImplXXXX_Shutdown().
// If you are new to ImGui, see examples/README.txt and documentation at the top of imgui.cpp.
// https://github.com/ocornut/imgui

#pragma once


struct GLFWwindow;

namespace ImGui {

class ImGuiImpl {
public:
	ImGuiImpl();

private:
	GLFWwindow *m_window;
	double m_time;

	bool m_mousePressed[3];
	float m_mouseWheel;

	unsigned int m_fontTexture;

public:
	void InitImpl(GLFWwindow *window);
	void ShutdownImpl();

	void NewFrame();
	void InvalidateDeviceObjects();
	bool CreateDeviceObjects();

	void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	void ScrollCallback(GLFWwindow *window, double xoffset, double yoffset);
	void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
	void CharCallback(GLFWwindow *window, unsigned int c);
};

} // End of namespace ImGui
