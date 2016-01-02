/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"


namespace Lantern {

struct GlobalArgs;


class Visualizer {
public:
	Visualizer(GlobalArgs *globalArgs);
	~Visualizer();

private:
	GlobalArgs *m_globalArgs;
	float3 *m_tempFrameBuffer;

private:
	void CopyFrameBufferToTemp();

public:
	void Run();
};

} // End of namespace Lantern
