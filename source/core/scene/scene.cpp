/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"

#include "scene/mesh_elements.h"

#include "math/vector_math.h"

#include "materials/matte.h"
#include "materials/media/non_scattering_medium.h"
#include "materials/media/isotropic_scattering_medium.h"
#include "materials/textures/constant_texture.h"
#include "materials/textures/image_texture.h"
#include "materials/textures/uv_texture.h"

#include "primitives/triangle_mesh.h"
#include "primitives/quad_mesh.h"
#include "primitives/grid.h"
#include "primitives/sphere.h"

#include "io/lantern_model_file.h"

#include "json.hpp"
#include "json_schema_validator.hpp"


#include <stdlib.h>


namespace Lantern {

Scene::Scene()
	: Camera(nullptr),
	  BackgroundColor(0.0f),
	  m_device(rtcNewDevice(nullptr)),
	  m_scene(nullptr) {
}

Scene::~Scene() {
	CleanupScene();

	rtcReleaseDevice(m_device);
}

bool Scene::LoadSceneFromJSON(const char *filePath) {
	std::error_code ec;
	m_jsonPath = std::filesystem::canonical(std::filesystem::path(filePath), ec);

	if (ec) {
		printf("Failed to open scene file - %s\n", ec.message().c_str());
		return false;
	}

	if (ParseJSON()) {
		rtcCommitScene(m_scene);
		return true;
	}

	return false;
}

bool Scene::ReloadSceneFromJSON() {
	// Cleanup from the old scene
	CleanupScene();

	if (ParseJSON()) {
		rtcCommitScene(m_scene);
		return true;
	}

	return false;
}

Primitive *Scene::RandomOneLight(UniformSampler *sampler) {
	// FIXME: Update to a full size_t if we ever get lots and lots of lights
	uint numLights = (uint)m_lights.size();
	if (numLights == 0) {
		return nullptr;
	}

	uint lightIndex = sampler->NextDiscrete(numLights);
	return m_lights[lightIndex];
}

void Scene::Intersect(RTCRayHit *ray) const {
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	context.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
	rtcIntersect1(m_scene, &context, ray);
}

void Scene::Occluded(RTCRay *ray) const {
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	context.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
	rtcOccluded1(m_scene, &context, ray);
}

void loader(const nlohmann::json_uri &uri, nlohmann::json &schema) {
	std::fstream lf("." + uri.path());
	if (!lf.good()) {
		throw std::invalid_argument("could not open " + uri.url() + " tried with " + uri.path());
	}

	lf >> schema;
}

bool Scene::ParseJSON() {
	// Load the schema
	nlohmann::json schema;

	// First, assume we're running from the 'build/...' directory, and try to load the schema from the scenes directory
	std::fstream lf("scene.schema.json");
	if (lf.good()) {
		try {
			lf >> schema;
		} catch (std::exception &e) {
			printf("%s at %zu - while parsing the schema\n", e.what(), lf.tellp());
			return false;
		}
	} else {
		lf.close();
		// Next, try to find the schema in the working directory
		lf.open("scene.schema.json");
		if (lf.good()) {
			try {
				lf >> schema;
			} catch (std::exception &e) {
				printf("%s at %zu - while parsing the schema\n", e.what(), lf.tellp());
				return false;
			}
		} else {
			// Barf
			printf("Failed to find scene.schema.json");
			return false;
		}
	}

	// Set up the validator
	nlohmann::json_schema_draft4::json_validator validator(loader);

	try {
		validator.set_root_schema(schema);
	} catch (const std::exception &e) {
		printf("Setting root schema failed: %s", e.what());
		return false;
	}


	m_scene = rtcNewScene(m_device);
	rtcSetSceneFlags(m_scene, RTC_SCENE_FLAG_NONE);
	rtcSetSceneBuildQuality(m_scene, RTC_BUILD_QUALITY_HIGH);

	std::ifstream ifs(m_jsonPath);
	if (!ifs.good()) {
		printf("Could not open %ls\n", m_jsonPath.c_str());
		return false;
	}
	nlohmann::json j;

	try {
		ifs >> j;
		validator.validate(j);
	} catch (std::exception &e) {
		printf("Schema validation failed: %s at offset %zu\n", e.what(), ifs.tellg());
		return false;
	}

	if (j.count("background_color") == 1) {
		BackgroundColor.x = j["background_color"][0].get<float>();
		BackgroundColor.y = j["background_color"][1].get<float>();
		BackgroundColor.z = j["background_color"][2].get<float>();
	}

	if (j.count("camera") != 1) {
		printf("JSON parse error: \"camera\" is required\n");
		return false;
	}

	// Get the camera values
	ParseCamera(j);

	std::unordered_map<std::string, BSDF *> bsdfMap;
	std::unordered_map<std::string, Medium *> mediaMap;

	ParseMaterials(j, &bsdfMap);
	ParseMedia(j, &mediaMap);
	if (!ParsePrimitives(j, bsdfMap, mediaMap)) {
		return false;
	}

	return true;
}

void Scene::CleanupScene() {
	delete Camera;

	for (auto &bsdf : m_bsdfs) {
		delete bsdf;
	}
	m_bsdfs.clear();
	for (auto &medium : m_media) {
		delete medium;
	}
	m_media.clear();
	for (auto &keyValue : m_primitives) {
		delete keyValue.second;
	}
	m_primitives.clear();
	m_lights.clear();

	m_imageCache.Clear();

	rtcReleaseScene(m_scene);
}

} // End of namespace Lantern
