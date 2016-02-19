/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"

#include "scene/mesh_elements.h"

#include <embree2/rtcore.h>


namespace Lantern {

Scene::Scene()
	: m_device(rtcNewDevice(nullptr)),
	  m_scene(rtcDeviceNewScene(m_device, RTC_SCENE_STATIC, RTC_INTERSECT1)) {
}

Scene::~Scene() {
	rtcDeleteScene(m_scene);
	rtcDeleteDevice(m_device);
}

void Scene::AddMesh(Mesh *mesh, Material *material) {
	uint meshId = rtcNewTriangleMesh(m_scene, RTC_GEOMETRY_STATIC, mesh->Triangles.size(), mesh->Vertices.size());
	m_meshes[meshId] = material;

	Vertex *vertices = (Vertex *)rtcMapBuffer(m_scene, meshId, RTC_VERTEX_BUFFER);
	memcpy(vertices, &mesh->Vertices[0], mesh->Vertices.size() * sizeof(Vertex));
	rtcUnmapBuffer(m_scene, meshId, RTC_VERTEX_BUFFER);

	Triangle* triangles = (Triangle*)rtcMapBuffer(m_scene, meshId, RTC_INDEX_BUFFER);
	memcpy(triangles, &mesh->Triangles[0], mesh->Triangles.size() * sizeof(Triangle));
	rtcUnmapBuffer(m_scene, meshId, RTC_INDEX_BUFFER);
}

} // End of namespace Lantern
