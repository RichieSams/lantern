/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"

#include "camera/pinhole_camera.h"

#include "scene/image_cache.h"

#define EMBREE_STATIC_LIB
#include "embree3/rtcore.h"

#include "json.hpp"

#include <unordered_map>
#include <filesystem>

namespace Lantern {

class BSDF;
class Medium;
struct Mesh;
struct LanternModelFile;
class Texture;
class Primitive;

class Scene {
public:
	Scene();
	~Scene();

public:
	PinholeCamera *Camera;
	float3 BackgroundColor;

private:
	std::filesystem::path m_jsonPath;
	
	std::vector<BSDF *> m_bsdfs;
	std::vector<Medium *> m_media;
	std::vector<Texture *> m_textures;

	ImageCache m_imageCache;

	std::unordered_map<uint, Primitive *> m_primitives;
	std::vector<Primitive *> m_lights;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	bool LoadSceneFromJSON(const char *filePath);
	bool ReloadSceneFromJSON();

	Primitive *GetPrimitive(uint geometryId) {
		return m_primitives[geometryId];
	}
	std::size_t NumLights() const { return m_lights.size(); }
	Primitive *RandomOneLight(UniformSampler *sampler);

	void Intersect(RTCRayHit *ray) const;
	void Occluded(RTCRay *ray) const;

private:
	bool ParseJSON();
	void ParseCamera(nlohmann::json &root);
	void ParseMaterials(nlohmann::json &root, std::unordered_map<std::string, BSDF *> *bsdfMap);
	void ParseMedia(nlohmann::json &root, std::unordered_map<std::string, Medium *> *mediaMap);
	bool ParsePrimitives(nlohmann::json &root, std::unordered_map<std::string, BSDF *> &bsdfMap, std::unordered_map<std::string, Medium *> &mediaMap);
	Texture *ParseTexture(nlohmann::json &root);
	void CleanupScene();
};

} // End of namespace Lantern
