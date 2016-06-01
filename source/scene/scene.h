/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"

#include "camera/pinhole_camera.h"

#include "scene/light.h"
#include "scene/ray.h"

#include <unordered_map>


struct __RTCDevice;
typedef __RTCDevice * RTCDevice;
struct __RTCScene;
typedef __RTCScene * RTCScene;

namespace Lantern {

struct Material;
struct Mesh;

class Scene {
public:
	Scene();
	~Scene();

public:
	PinholeCamera Camera;
	float3 BackgroundColor;

private:
	std::unordered_map<uint, Material *> m_materials;
	std::vector<Light *> m_lightList;
	std::unordered_map<uint, Light *> m_lightMap;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	void SetCamera(float phi, float theta, float radius, float clientWidth, float clientHeight, float fov = M_PI_2) {
		Camera = PinholeCamera(phi, theta, radius, clientWidth, clientHeight, fov);
	}

	void AddMesh(Mesh *mesh, Material *material);
	void AddMesh(Mesh *mesh, Material *material, float3 color, float radiantPower);
	void Commit() const;

	Material *GetMaterial(uint meshId) {
		return m_materials[meshId];
	}
	Light *GetLight(uint meshId) {
		auto iter = m_lightMap.find(meshId);
		if (iter != m_lightMap.end()) {
			return iter->second;
		} else {
			return nullptr;
		}
	}
	std::size_t NumLights() const { return m_lightList.size(); }
	Light *RandomOneLight(UniformSampler *sampler);

	void Intersect(Ray &ray) const;
	float3 InterpolateNormal(uint meshId, uint primId, float u, float v) const;

private:
	uint AddMeshInternal(Mesh *mesh, Material *material);
};

} // End of namespace Lantern
