/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"

#include "scene/mesh_elements.h"
#include "scene/area_light.h"

#include "bsdfs/bsdf.h"

#include "math/vector_math.h"

#include <embree2/rtcore.h>


namespace Lantern {

Scene::Scene()
	: BackgroundColor(0.0f),
	  m_device(rtcNewDevice(nullptr)),
	  m_scene(rtcDeviceNewScene(m_device, RTC_SCENE_STATIC, RTC_INTERSECT1 | RTC_INTERPOLATE)) {
}

Scene::~Scene() {
	rtcDeleteScene(m_scene);
	rtcDeleteDevice(m_device);
}

uint Scene::AddMeshInternal(Mesh *mesh, BSDF *bsdf) {
	uint meshId = rtcNewTriangleMesh(m_scene, RTC_GEOMETRY_STATIC, mesh->Indices.size() / 3, mesh->Positions.size());
	m_bsdfs[meshId] = bsdf;

	float3a *vertices = (float3a *)rtcMapBuffer(m_scene, meshId, RTC_VERTEX_BUFFER);
	memcpy(vertices, &mesh->Positions[0], mesh->Positions.size() * sizeof(float3a));
	rtcUnmapBuffer(m_scene, meshId, RTC_VERTEX_BUFFER);

	uint *indices = (uint *)rtcMapBuffer(m_scene, meshId, RTC_INDEX_BUFFER);
	memcpy(indices, &mesh->Indices[0], mesh->Indices.size() * sizeof(int));
	rtcUnmapBuffer(m_scene, meshId, RTC_INDEX_BUFFER);

	float3 *normals = (float3 *)_aligned_malloc(sizeof(float3) * mesh->Normals.size(), 16);
	memcpy(normals, &mesh->Normals[0], sizeof(float3) * mesh->Normals.size());
	rtcSetBuffer(m_scene, meshId, RTC_USER_VERTEX_BUFFER0, normals, 0u, sizeof(float3));

	return meshId;
}

void Scene::AddMesh(Mesh *mesh, BSDF *bsdf) {
	AddMeshInternal(mesh, bsdf);
}

void Scene::AddMesh(Mesh *mesh, BSDF *bsdf, float3 color, float radiantPower) {
	uint meshId = AddMeshInternal(mesh, bsdf);
	
	// Calculate the surface area
	std::vector<float3a> *vertices = (std::vector<float3a> *)&mesh->Positions;
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

void Scene::Commit() const {
	rtcCommit(m_scene);
}

Light *Scene::RandomOneLight(UniformSampler *sampler) {
	uint numLights = m_lightList.size();
	if (numLights == 0) {
		return nullptr;
	}

	uint lightIndex = sampler->NextFloat() * numLights;
	return m_lightList[lightIndex];
}

void Scene::Intersect(Ray &ray) const {
	rtcIntersect(m_scene, ray);
}

float3 Scene::InterpolateNormal(uint meshId, uint primId, float u, float v) const {
	float3 normal;
	rtcInterpolate(m_scene, meshId, primId, u, v, RTC_USER_VERTEX_BUFFER0, &normal.x, nullptr, nullptr, 3);

	if (AnyNan(normal)) {
		printf("nan");
	}

	return normal;
}

} // End of namespace Lantern
