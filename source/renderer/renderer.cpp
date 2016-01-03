/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "renderer/renderer.h"

#include "common/global_args.h"

#include "scene/mesh_elements.h"

#include "graphics/atomic_frame_buffer.h"


namespace Lantern {

Renderer::Renderer(GlobalArgs *globalArgs)
	: m_globalArgs(globalArgs),
	  m_device(rtcNewDevice(nullptr)),
	  m_scene(nullptr) {
}

Renderer::~Renderer() {
	if (!m_scene) {
		rtcDeleteScene(m_scene);
	}
	rtcDeleteDevice(m_device);
}

void Renderer::SetScene() {
	m_scene = rtcDeviceNewScene(m_device, RTC_SCENE_STATIC, RTC_INTERSECT1);

	// Create a cube
	uint id = rtcNewTriangleMesh(m_scene, RTC_GEOMETRY_STATIC, 12, 8);

	Vertex *vertices = (Vertex *)rtcMapBuffer(m_scene, id, RTC_VERTEX_BUFFER);
	vertices[0].X = -1; vertices[0].Y = -1; vertices[0].Z = -1;
	vertices[1].X = -1; vertices[1].Y = -1; vertices[1].Z = +1;
	vertices[2].X = -1; vertices[2].Y = +1; vertices[2].Z = -1;
	vertices[3].X = -1; vertices[3].Y = +1; vertices[3].Z = +1;
	vertices[4].X = +1; vertices[4].Y = -1; vertices[4].Z = -1;
	vertices[5].X = +1; vertices[5].Y = -1; vertices[5].Z = +1;
	vertices[6].X = +1; vertices[6].Y = +1; vertices[6].Z = -1;
	vertices[7].X = +1; vertices[7].Y = +1; vertices[7].Z = +1;
	rtcUnmapBuffer(m_scene, id, RTC_VERTEX_BUFFER);


	Triangle* triangles = (Triangle*)rtcMapBuffer(m_scene, id, RTC_INDEX_BUFFER);

	// left side
	triangles[0].V0 = 0; triangles[0].V1 = 2; triangles[0].V2 = 1;
	triangles[1].V0 = 1; triangles[1].V1 = 2; triangles[1].V2 = 3;

	// right side
	triangles[2].V0 = 4; triangles[2].V1 = 5; triangles[2].V2 = 6;
	triangles[3].V0 = 5; triangles[3].V1 = 7; triangles[3].V2 = 6;

	// bottom side
	triangles[4].V0 = 0; triangles[4].V1 = 1; triangles[4].V2 = 4;
	triangles[5].V0 = 1; triangles[5].V1 = 5; triangles[5].V2 = 4;

	// top side
	triangles[6].V0 = 2; triangles[6].V1 = 6; triangles[6].V2 = 3;
	triangles[7].V0 = 3; triangles[7].V1 = 6; triangles[7].V2 = 7;

	// front side
	triangles[8].V0 = 0; triangles[8].V1 = 4; triangles[8].V2 = 2;
	triangles[9].V0 = 2; triangles[9].V1 = 4; triangles[9].V2 = 6;

	// back side
	triangles[10].V0 = 1; triangles[10].V1 = 3; triangles[10].V2 = 5;
	triangles[11].V0 = 3; triangles[11].V1 = 7; triangles[11].V2 = 5;

	rtcUnmapBuffer(m_scene, id, RTC_INDEX_BUFFER);


	rtcCommit(m_scene);
}

void Renderer::Start() {
	uint width = m_globalArgs->FrameBuffer->Width();
	uint height = m_globalArgs->FrameBuffer->Height();

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			float3 color;
			if (y >= 0 && y < 100) {
				color = float3(1.0f, 0.0f, 0.0f);
			} else if (y >= 100 && y < 200) {
				color = float3(1.0f, 1.0f, 0.0f);
			} else if (y >= 200 && y < 300) {
				color = float3(0.0f, 1.0f, 0.0f);
			} else if (y >= 300 && y < 400) {
				color = float3(1.0f, 1.0f, 1.0f);
			} else if (y >= 400 && y < 500) {
				color = float3(1.0f, 0.0f, 1.0f);
			} else if (y >= 500 && y < 600) {
				color = float3(0.0f, 0.0f, 0.0f);
			} else if (y >= 600 && y < 700) {
				color = float3(0.0f, 0.0f, 1.0f);
			} else {
				color = float3(0.5f, 1.0f, 0.5f);
			}

			m_globalArgs->FrameBuffer->SplatPixel(x, y, color);
		}
	}

	m_globalArgs->RenderChanged.store(true);
}

} // End of namespace Lantern
