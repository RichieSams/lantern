/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"

#include "camera/thin_lens_camera.h"

#include "scene/light.h"

#include <embree2/rtcore.h>

#include <unordered_map>


struct __RTCScene;
typedef __RTCScene * RTCScene;

namespace Lantern {

struct Mesh;
class Material;

class Scene {
public:
	Scene();
	~Scene();

public:
	ThinLensCamera Camera;

private:
	std::unordered_map<uint, Material *> m_materials;
	std::vector<Light *> m_lightList;
	std::unordered_map<uint, Light *> m_lightMap;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	void SetCamera(float phi, float theta, float radius, float clientWidth, float clientHeight, float fov = M_PI_2) {
		Camera = ThinLensCamera(phi, theta, radius, clientWidth, clientHeight, fov);
	}

	void AddMesh(Mesh *mesh, Material *material);
	void AddMesh(Mesh *mesh, Material *material, float3 color, float radiantPower);
	void Commit() { rtcCommit(m_scene);	}

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
	std::size_t NumLights() { return m_lightList.size(); }
	Light *RandomOneLight(UniformSampler *sampler);

	void Intersect(RTCRay &ray) const { rtcIntersect(m_scene, ray); }

private:
	uint AddMeshInternal(Mesh *mesh, Material *material);
};

} // End of namespace Lantern
