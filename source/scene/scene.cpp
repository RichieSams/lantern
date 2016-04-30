/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"

#include "scene/mesh_elements.h"
#include "scene/area_light.h"

#include "materials/material.h"

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

uint Scene::AddMeshInternal(Mesh *mesh, Material *material) {
	uint meshId = rtcNewTriangleMesh(m_scene, RTC_GEOMETRY_STATIC, mesh->Indices.size() / 3, mesh->Vertices.size());
	m_materials[meshId] = material;

	Vertex *vertices = (Vertex *)rtcMapBuffer(m_scene, meshId, RTC_VERTEX_BUFFER);
	memcpy(vertices, &mesh->Vertices[0], mesh->Vertices.size() * sizeof(Vertex));
	rtcUnmapBuffer(m_scene, meshId, RTC_VERTEX_BUFFER);

	uint *indices = (uint *)rtcMapBuffer(m_scene, meshId, RTC_INDEX_BUFFER);
	memcpy(indices, &mesh->Indices[0], mesh->Indices.size() * sizeof(int));
	rtcUnmapBuffer(m_scene, meshId, RTC_INDEX_BUFFER);

	return meshId;
}

void Scene::AddMesh(Mesh *mesh, Material *material) {
	AddMeshInternal(mesh, material);
}

void Scene::AddMesh(Mesh *mesh, Material *material, float3 color, float radiantPower) {
	uint meshId = AddMeshInternal(mesh, material);
	
	// Calculate the surface area
	assert(sizeof(Vertex) == sizeof(float3a));
	std::vector<float3a> *vertices = (std::vector<float3a> *)&mesh->Vertices;
	std::vector<int> *indices = &mesh->Indices;

	float totalArea = 0.0f;
	for (std::size_t i = 0; i < indices->size(); i += 3) {
		float3a v0 = (*vertices)[(*indices)[i]];
		float3a v1 = (*vertices)[(*indices)[i + 1]];
		float3a v2 = (*vertices)[(*indices)[i + 2]];

		totalArea += 0.5f * length(cross(v0 - v1, v0 - v2));
	}

	AreaLight *light = new AreaLight(color, radiantPower, totalArea, meshId, mesh->BoundingSphere);
	m_lightList.push_back(light);
	m_lightMap[meshId] = light;
}

Light *Scene::RandomOneLight(UniformSampler *sampler) {
	uint numLights = m_lightList.size();
	if (numLights == 0) {
		return nullptr;
	}

	uint lightIndex = sampler->NextFloat() * numLights;
	return m_lightList[lightIndex];
}


} // End of namespace Lantern
