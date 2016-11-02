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
class Medium;
struct Material;
struct Mesh;
struct LanternModelFile;

class Scene {
public:
	Scene();
	~Scene();

public:
	PinholeCamera *Camera;
	float3 BackgroundColor;

private:
	std::vector<BSDF *> m_bsdfs;
	std::vector<Medium *> m_media;
	std::vector<Material *> m_materials;
	std::vector<float *> m_meshNormals;
	std::vector<Light *> m_lights;

	struct Model {
		Model() : Material(nullptr), Light(nullptr) { }
		Model(Material *material, Light *light = nullptr)
			: Material(material),
			  Light(light) {
		}

		Material *Material;
		Light *Light;
	};
	std::unordered_map<uint, Model> m_models;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	bool LoadSceneFromJSON(const char *filePath);
	void Commit() const;

	Material *GetMaterial(uint modelId) {
		return m_models[modelId].Material;
	}
	Light *GetLight(uint modelId) {
		return m_models[modelId].Light;
	}
	std::size_t NumLights() const { return m_lights.size(); }
	Light *RandomOneLight(UniformSampler *sampler);

	void Intersect(Ray &ray) const;
	float3 InterpolateNormal(uint meshId, uint primId, float u, float v) const;

private:
	uint AddMesh(Mesh *mesh, float4x4 &transform, float *out_surfaceArea, float4 *out_boundingSphere);
	uint AddLMF(LanternModelFile *lmf, float4x4 &transform, float *out_surfaceArea, float4 *out_boundingSphere);
};

} // End of namespace Lantern
