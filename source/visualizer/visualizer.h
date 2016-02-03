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

class Visualizer {
public:
	Visualizer(Renderer *renderer);
	~Visualizer();

private:
	Renderer *m_renderer;
	float3 *m_tempFrameBuffer;

	GLFWwindow *m_window;

	ImGui::ImGuiImpl m_imGuiImpl;

public:
	void Run();

private:
	void Init();
	void Shutdown();
	void CopyFrameBufferToGPU();
};

} // End of namespace Lantern
