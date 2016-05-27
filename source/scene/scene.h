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

class BSDF;
struct Mesh;

class Scene {
public:
	Scene();
	~Scene();

public:
	PinholeCamera Camera;
	float3 BackgroundColor;

private:
	std::unordered_map<uint, BSDF *> m_bsdfs;
	std::vector<Light *> m_lightList;
	std::unordered_map<uint, Light *> m_lightMap;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	void SetCamera(float phi, float theta, float radius, float clientWidth, float clientHeight, float fov = M_PI_2) {
		Camera = PinholeCamera(phi, theta, radius, clientWidth, clientHeight, fov);
	}

	void AddMesh(Mesh *mesh, BSDF *bsdf);
	void AddMesh(Mesh *mesh, BSDF *bsdf, float3 color, float radiantPower);
	void Commit() const;

	BSDF *GetBSDF(uint meshId) {
		return m_bsdfs[meshId];
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

	void Intersect(RTCRay &ray) const;

private:
	uint AddMeshInternal(Mesh *mesh, BSDF *bsdf);
};

} // End of namespace Lantern
