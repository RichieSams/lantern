/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "renderer/renderer.h"

#include "scene/mesh_elements.h"
#include "scene/ray.h"


namespace Lantern {

Renderer::Renderer(uint frameWidth, uint frameHeight)
	: m_frameBuffer(frameWidth, frameHeight),
	  m_camera(frameWidth, frameHeight),
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
	vertices[0].x = -1; vertices[0].y = -1; vertices[0].z = -1;
	vertices[1].x = -1; vertices[1].y = -1; vertices[1].z = +1;
	vertices[2].x = -1; vertices[2].y = +1; vertices[2].z = -1;
	vertices[3].x = -1; vertices[3].y = +1; vertices[3].z = +1;
	vertices[4].x = +1; vertices[4].y = -1; vertices[4].z = -1;
	vertices[5].x = +1; vertices[5].y = -1; vertices[5].z = +1;
	vertices[6].x = +1; vertices[6].y = +1; vertices[6].z = -1;
	vertices[7].x = +1; vertices[7].y = +1; vertices[7].z = +1;
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

void Renderer::RenderFrame() {
	uint width = m_frameBuffer.Width;
	uint height = m_frameBuffer.Height;

	for (uint y = 0; y < height; ++y) {
		for (uint x = 0; x < width; ++x) {
			RTCRay ray = m_camera.CalculateRayFromPixel(x, y);
			
			rtcIntersect(m_scene, ray);

			float3a color(0.0f);
			if (ray.geomID != RTC_INVALID_GEOMETRY_ID) {
				float3a lightDir = normalize(float3a(-1, -1, -1));

				/* initialize shadow ray */
				RTCRay shadow;
				shadow.org = ray.org + ray.tfar * ray.dir;
				shadow.dir = -lightDir;
				shadow.tnear = 0.001f;
				shadow.tfar = embree::inf;
				shadow.geomID = RTC_INVALID_GEOMETRY_ID;
				shadow.primID = RTC_INVALID_GEOMETRY_ID;
				shadow.instID = RTC_INVALID_GEOMETRY_ID;
				shadow.mask = 0xFFFFFFFF;
				shadow.time = 0.0f;

				/* trace shadow ray */
				rtcOccluded(m_scene, shadow);

				/* add light contribution */
				if (shadow.geomID != 0) {
					color = float3a(1.0f) * embree::clamp(-dot(lightDir, normalize(ray.Ng)), 0.0f, 1.0f);
				}
			}

			m_frameBuffer.SplatPixel(x, y, color);
		}
	}
}

} // End of namespace Lantern
