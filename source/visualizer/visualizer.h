/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/typedefs.h"

#include "math/vector_types.h"


struct GLFWwindow;

namespace Lantern {

struct GlobalArgs;

class Visualizer {
public:
	Visualizer(GlobalArgs *globalArgs);
	~Visualizer();

private:
	GlobalArgs *m_globalArgs;
	float3 *m_tempFrameBuffer;

	GLFWwindow *m_window;

	int m_clientWidth;
	int m_clientHeight;

public:
	void Run();

private:
	void Init();
	void Shutdown();
	void CopyFrameBufferToGPU();
};

} // End of namespace Lantern
