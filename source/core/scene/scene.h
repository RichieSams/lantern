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
#include "scene/image_cache.h"

#include <unordered_map>
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;


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
class Texture;

class Scene {
public:
	Scene();
	~Scene();

public:
	PinholeCamera *Camera;
	float3 BackgroundColor;

private:
	fs::path m_jsonPath;
	
	std::vector<BSDF *> m_bsdfs;
	std::vector<Medium *> m_media;
	std::vector<Material *> m_materials;
	std::vector<Texture *> m_textures;
	std::vector<float *> m_meshNormals;
	std::vector<float *> m_meshTexCoords;
	std::vector<Light *> m_lights;

	ImageCache m_imageCache;

	struct Model {
		Model() : material(nullptr), light(nullptr) { }
		Model(Material *material, Light *light = nullptr)
			: material(material),
			  light(light) {
		}

		Material *material;
		Light *light;
		bool hasNormals;
		bool hasTexCoords;
	};
	std::unordered_map<uint, Model> m_models;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	bool LoadSceneFromJSON(std::string &filePath);
	bool ReloadSceneFromJSON();

	Material *GetMaterial(uint modelId) {
		return m_models[modelId].material;
	}
	Light *GetLight(uint modelId) {
		return m_models[modelId].light;
	}
	std::size_t NumLights() const { return m_lights.size(); }
	Light *RandomOneLight(UniformSampler *sampler);

	void Intersect(Ray &ray) const;
	bool HasNormals(uint meshId) {
		return m_models[meshId].hasNormals;
	}
	float3 InterpolateNormal(uint meshId, uint primId, float u, float v) const;
	bool HasTexCoords(uint meshId) {
		return m_models[meshId].hasTexCoords;
	}
	float2 InterpolateTexCoord(uint meshId, uint primId, float u, float v) const;

private:
	bool ParseJSON();
	uint AddMesh(Mesh *mesh, float4x4 &transform, float *out_surfaceArea, float4 *out_boundingSphere, bool *out_hasNormals, bool *out_hasTexCoords);
	uint AddLMF(LanternModelFile *lmf, float4x4 &transform, float *out_surfaceArea, float4 *out_boundingSphere, bool *out_hasNormals, bool *out_hasTexCoords);
	void CleanupScene();
};

} // End of namespace Lantern
