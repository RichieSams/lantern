/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "graphics/frame_buffer.h"

#include "scene/camera.h"

#include <embree2/rtcore.h>


namespace Lantern {
struct GlobalArgs;

class Renderer {
public:
	Renderer(uint frameWidth, uint frameHeight);
	~Renderer();

private:
	FrameBuffer m_frameBuffer;
	Camera m_camera;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	void SetScene();
	void RenderFrame();
	FrameBuffer *GetFrameBuffer() { return &m_frameBuffer; }

};

} // End of namespace Lantern
