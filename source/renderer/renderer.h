/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include <embree2/rtcore.h>


namespace Lantern {

class AtomicFrameBuffer;

class Renderer {
public:
	Renderer(AtomicFrameBuffer *frameBuffer);
	~Renderer();

private:
	AtomicFrameBuffer *m_frameBuffer;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	void Start();

};

} // End of namespace Lantern
