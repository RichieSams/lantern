/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "renderer/renderer.h"

#include "common/typedefs.h"
#include "common/mesh_elements.h"
#include "common/atomic_frame_buffer.h"


namespace Lantern {

Renderer::Renderer(AtomicFrameBuffer *frameBuffer)
	: m_frameBuffer(frameBuffer),
	  m_device(rtcNewDevice(nullptr)),
	  m_scene(nullptr) {
}

Renderer::~Renderer() {
	if (!m_scene) {
		rtcDeleteScene(m_scene);
	}
	rtcDeleteDevice(m_device);
}

void Renderer::Start() {
	uint width = m_frameBuffer->Width();
	uint height = m_frameBuffer->Height();

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			m_frameBuffer->SplatPixel(x, y, float3(1.0f, 0.0f, 0.0f));
		}
	}
}

} // End of namespace Lantern
