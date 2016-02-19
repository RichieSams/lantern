/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/typedefs.h"

#include "scene/thin_lens_camera.h"

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
	std::unordered_map<uint, Material *> m_meshes;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	void SetCamera(float phi, float theta, float radius, float clientWidth, float clientHeight, float fov = M_PI_2) {
		Camera = ThinLensCamera(phi, theta, radius, clientWidth, clientHeight, fov);
	}

	void AddMesh(Mesh *mesh, Material *material);
	Material *GetMaterial(uint meshId) { return m_meshes[meshId]; }
	void Commit() { rtcCommit(m_scene);	}

	void Intersect(RTCRay &ray) const { rtcIntersect(m_scene, ray); }
};

} // End of namespace Lantern
