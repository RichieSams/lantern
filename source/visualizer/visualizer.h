/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/vector_types.h"


namespace Lantern {

class AtomicFrameBuffer;


class Visualizer {
public:
	Visualizer(AtomicFrameBuffer *frameBuffer);
	~Visualizer();

private:
	AtomicFrameBuffer *m_atomicFrameBuffer;
	float3 *m_tempFrameBuffer;

private:
	void CopyAtomicFrameBufferToTemp();

public:
	void Run();
};

} // End of namespace Lantern
