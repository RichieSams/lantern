/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"

#include "scene/mesh_elements.h"
#include "scene/area_light.h"
#include "scene/geometry_generator.h"

#include "math/vector_math.h"

#include "materials/material.h"
#include "materials/bsdfs/ideal_specular_dielectric.h"
#include "materials/bsdfs/lambert_bsdf.h"
#include "materials/bsdfs/mirror_bsdf.h"
#include "materials/media/non_scattering_medium.h"
#include "materials/media/isotropic_scattering_medium.h"
#include "materials/textures/constant_texture.h"
#include "materials/textures/image_texture.h"
#include "materials/textures/uv_texture.h"

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

float4 CalculateBoundingSphere(float3a *positions, std::size_t len) {
	float3a center = positions[0];
	float radius = 0.0001f;
	float3a pos, diff;
	float length, alpha, alphaSq;

	for (std::size_t i = 0; i < 2; i++) {
		for (std::size_t j = 0; j < len; ++j) {
			pos = positions[j];
			diff = pos - center;
			length = embree::length(diff);
			if (length > radius) {
				alpha = length / radius;
				alphaSq = alpha * alpha;
				radius = 0.5f * (alpha + 1 / alpha) * radius;
				center = 0.5f * ((1 + 1 / alphaSq) * center + (1 - 1 / alphaSq) * pos);
			}
		}
	}

	for (std::size_t j = 0; j < len; ++j) {
		pos = positions[j];
		diff = pos - center;
		length = embree::length(diff);
		if (length > radius) {
			radius = (radius + length) / 2.0f;
			center = center + ((length - radius) / length * diff);
		}
	}

	return float4(center, radius);
}

bool Scene::LoadSceneFromJSON(const char *filePath) {
	m_jsonPath = canonical(fs::path(filePath));

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

Light *Scene::RandomOneLight(UniformSampler *sampler) {
	// FIXME: Update to a full size_t if we ever get lots and lots of lights
	uint numLights = (uint)m_lights.size();
	if (numLights == 0) {
		return nullptr;
	}

	uint lightIndex = sampler->NextDiscrete(numLights);
	return m_lights[lightIndex];
}

void Scene::Intersect(RTCRayHit &ray) const {
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);
	context.flags = RTC_INTERSECT_CONTEXT_FLAG_INCOHERENT;
	rtcIntersect1(m_scene, &context, &ray);
}

float3 Scene::InterpolateNormal(uint meshId, uint primId, float u, float v) const {
	float3 normal;
	rtcInterpolate1(rtcGetGeometry(m_scene, meshId), primId, u, v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, nullptr, nullptr, 3);

	if (AnyNan(normal)) {
		printf("nan normal");
	}

	return normal;
}

float2 Scene::InterpolateTexCoord(uint meshId, uint primId, float u, float v) const {
	float2 texCoord;
	rtcInterpolate1(rtcGetGeometry(m_scene, meshId), primId, u, v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &texCoord.x, nullptr, nullptr, 2);

	if (AnyNan(texCoord)) {
		printf("nan texCoord");
	}

	return texCoord;
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
	std::fstream lf("../../../../scenes/scene.schema.json");
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
	nlohmann::json camera = j["camera"];

	float3 position = float3(camera["position"][0].get<float>(), camera["position"][1].get<float>(), camera["position"][2].get<float>());
	float3 target = float3(camera["target"][0].get<float>(), camera["target"][1].get<float>(), camera["target"][2].get<float>());
	float3 up = float3(camera["up"][0].get<float>(), camera["up"][1].get<float>(), camera["up"][2].get<float>());
	uint clientWidth = camera["client_width"].get<uint>();
	uint clientHeight = camera["client_height"].get<uint>();

	float fov = (float)M_PI_4;
	if (camera.count("fov") == 1) {
		fov = camera["fov"].get<float>();
	}

	Camera = new PinholeCamera(position, target, up, clientWidth, clientHeight, fov);

	std::unordered_map<std::string, BSDF *> bsdfMap;
	std::unordered_map<std::string, Medium *> mediaMap;
	std::unordered_map<std::string, Material *> materialMap;
	std::unordered_map<std::string, uint> primitiveMap;


	if (j.count("bsdfs") == 1) {
		for (auto &bsdf : j["bsdfs"]) {
			std::string name = bsdf["name"].get<std::string>();
			std::string type = bsdf["type"].get<std::string>();
			if (type == "ideal_specular_dielectric") {
				Texture *newTexture;
				if (bsdf["albedo"]["type"] == "constant") {
					newTexture = new ConstantTexture(float3(bsdf["albedo"]["value"][0].get<float>(),
					                                        bsdf["albedo"]["value"][1].get<float>(),
					                                        bsdf["albedo"]["value"][2].get<float>()));
				} else if (bsdf["albedo"]["type"] == "image") {
					uint imageId = m_imageCache.AddImage(bsdf["albedo"]["file_path"].get<std::string>().c_str());
					newTexture = new ImageTexture(&m_imageCache, imageId);
				} else if (bsdf["albedo"]["type"] == "uv") {
					newTexture = new UVTexture();
				}
				BSDF *newBSDF = new IdealSpecularDielectric(newTexture,
				                                            bsdf["ior"].get<float>());
				m_bsdfs.push_back(newBSDF);
				m_textures.push_back(newTexture);
				bsdfMap[name] = newBSDF;
			} else if (type == "lambert") {
				Texture *newTexture;
				if (bsdf["albedo"]["type"] == "constant") {
					newTexture = new ConstantTexture(float3(bsdf["albedo"]["value"][0].get<float>(),
					                                        bsdf["albedo"]["value"][1].get<float>(),
					                                        bsdf["albedo"]["value"][2].get<float>()));
				} else if (bsdf["albedo"]["type"] == "image") {
					uint imageId = m_imageCache.AddImage(bsdf["albedo"]["file_path"].get<std::string>().c_str());
					newTexture = new ImageTexture(&m_imageCache, imageId);
				} else if (bsdf["albedo"]["type"] == "uv") {
					newTexture = new UVTexture();
				}
				BSDF *newBSDF = new LambertBSDF(newTexture);
				m_bsdfs.push_back(newBSDF);
				m_textures.push_back(newTexture);
				bsdfMap[name] = newBSDF;
			} else if (type == "mirror") {
				Texture *newTexture;
				if (bsdf["albedo"]["type"] == "constant") {
					newTexture = new ConstantTexture(float3(bsdf["albedo"]["value"][0].get<float>(),
					                                        bsdf["albedo"]["value"][1].get<float>(),
					                                        bsdf["albedo"]["value"][2].get<float>()));
				} else if (bsdf["albedo"]["type"] == "image") {
					uint imageId = m_imageCache.AddImage(bsdf["albedo"]["file_path"].get<std::string>().c_str());
					newTexture = new ImageTexture(&m_imageCache, imageId);
				} else if (bsdf["albedo"]["type"] == "uv") {
					newTexture = new UVTexture();
				}
				BSDF *newBSDF = new MirrorBSDF(newTexture);
				m_bsdfs.push_back(newBSDF);
				m_textures.push_back(newTexture);
				bsdfMap[name] = newBSDF;
			}
		}
	}

	if (j.count("media") == 1) {
		for (auto &medium : j["media"]) {
			std::string name = medium["name"].get<std::string>();
			std::string type = medium["type"].get<std::string>();
			if (type == "non_scattering") {
				Medium *newMedia = new NonScatteringMedium(float3(medium["absorption_color"][0].get<float>(),
				                                                  medium["absorption_color"][1].get<float>(),
				                                                  medium["absorption_color"][2].get<float>()),
				                                           medium["absorption_at_distance"].get<float>());
				m_media.push_back(newMedia);
				mediaMap[name] = newMedia;
			} else if (type == "isotropic_scattering") {
				Medium *newMedia = new IsotropicScatteringMedium(float3(medium["absorption_color"][0].get<float>(),
				                                                        medium["absorption_color"][1].get<float>(),
				                                                        medium["absorption_color"][2].get<float>()),
				                                                 medium["absorption_at_distance"].get<float>(),
				                                                 medium["scattering_distance"].get<float>());
				m_media.push_back(newMedia);
				mediaMap[name] = newMedia;
			}
		}
	}

	if (j.count("materials") == 1) {
		for (auto &material : j["materials"]) {
			std::string name = material["name"].get<std::string>();
			std::string bsdfName = material["bsdf"].get<std::string>();

			if (bsdfMap.find(bsdfName) == bsdfMap.end()) {
				printf("BSDF [%s] could not be found for Material [%s]\n", bsdfName.c_str(), name.c_str());
				continue;
			}

			Medium *medium = nullptr;
			if (material.count("medium") == 1) {
				std::string mediaName = material["medium"].get<std::string>();
				if (mediaMap.find(mediaName) == mediaMap.end()) {
					printf("Medium [%s] could not be found for Material [%s]\n", mediaName.c_str(), name.c_str());
					continue;
				}

				medium = mediaMap[mediaName];
			}

			Material *newMaterial = new Material(bsdfMap[bsdfName], medium);
			m_materials.push_back(newMaterial);
			materialMap[name] = newMaterial;
		}
	}

	if (j.count("primitives") == 1) {
		for (auto &primitive : j["primitives"]) {
			std::string name = primitive["name"].get<std::string>();

			float4x4 transform(embree::one);
			if (primitive.count("transform") == 1) {
				nlohmann::json t = primitive["transform"];
				transform = float4x4(t[0].get<float>(), t[1].get<float>(), t[2].get<float>(), t[3].get<float>(),
				                     t[4].get<float>(), t[5].get<float>(), t[6].get<float>(), t[7].get<float>(),
				                     t[8].get<float>(), t[9].get<float>(), t[10].get<float>(), t[11].get<float>(),
				                     t[12].get<float>(), t[13].get<float>(), t[14].get<float>(), t[15].get<float>());
			}

			std::string type = primitive["type"].get<std::string>();
			uint meshId;
			float surfaceArea;
			float4 boundingSphere;
			bool hasNormals;
			bool hasTexCoords;
			if (type == "lmf") {
				std::string lmfFilePathString = primitive["file_path"].get<std::string>();
				fs::path lmfFilePath(lmfFilePathString);
				if (lmfFilePath.is_relative()) {
					lmfFilePath = m_jsonPath.parent_path() / lmfFilePath;
				}

				FILE *file = fopen(lmfFilePath.u8string().c_str(), "rb");
				if (!file) {
					perror("Error");
					printf("Unable to open \"%s\" for reading\n", lmfFilePath.u8string().c_str());
					continue;
				}

				LanternModelFile lmf;
				if (!ReadLMF(file, &lmf)) {
					continue;
				}
				fclose(file);

				meshId = AddLMF(&lmf, transform, &surfaceArea, &boundingSphere, &hasNormals, &hasTexCoords);
				primitiveMap[name] = meshId;
			} else if (type == "grid") {
				float width = primitive["width"].get<float>();
				float depth = primitive["depth"].get<float>();
				uint n = primitive["n"].get<uint>();
				uint m = primitive["m"].get<uint>();

				Mesh mesh;
				CreateGrid(width, depth, m, n, &mesh);
				meshId = AddMesh(&mesh, transform, &surfaceArea, &boundingSphere, &hasNormals, &hasTexCoords);
				primitiveMap[name] = meshId;
			} else if (type == "geosphere") {
				float radius = primitive["radius"].get<float>();
				uint n = primitive["n"].get<uint>();

				Mesh mesh;
				CreateGeosphere(radius, n, &mesh);
				meshId = AddMesh(&mesh, transform, &surfaceArea, &boundingSphere, &hasNormals, &hasTexCoords);
				primitiveMap[name] = meshId;
			} else {
				printf("Unknown primitive type: [%s]\n", type.c_str());
				continue;
			}

			std::string material = primitive["material"].get<std::string>();
			m_models[meshId].material = materialMap[material];
			m_models[meshId].hasNormals = hasNormals;
			m_models[meshId].hasTexCoords = hasTexCoords;

			if (primitive.count("emission") == 1) {
				float3 color(primitive["emission"]["color"][0].get<float>(), primitive["emission"]["color"][1].get<float>(), primitive["emission"]["color"][2].get<float>());
				float radiantPower = primitive["emission"]["radiant_power"].get<float>();

				AreaLight *light = new AreaLight(color, radiantPower, surfaceArea, meshId, boundingSphere);
				m_lights.push_back(light);
				m_models[meshId].light = light;
			}
		}
	}

	return true;
}

uint Scene::AddMesh(Mesh *mesh, float4x4 &transform, float *out_surfaceArea, float4 *out_boundingSphere, bool *out_hasNormals, bool *out_hasTexCoords) {
	RTCGeometry geometry = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetGeometryBuildQuality(geometry, RTC_BUILD_QUALITY_HIGH);
	rtcSetGeometryTimeStepCount(geometry, 1);

	float3a *vertices = (float3a *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(float3a), mesh->Positions.size());
	for (uint i = 0, j = 0; j < mesh->Positions.size(); ++i, ++j) {
		float3a vertex(mesh->Positions[i], 1.0f);

		vertices[j] = transform * vertex;
	}

	// Calculate the surface area
	*out_surfaceArea = 0.0f;
	for (std::size_t i = 0; i < mesh->Indices.size(); i += 3) {
		float3a v0 = vertices[mesh->Indices[i]];
		float3a v1 = vertices[mesh->Indices[i + 1]];
		float3a v2 = vertices[mesh->Indices[i + 2]];

		*out_surfaceArea += 0.5f * length(cross(v0 - v1, v0 - v2));
	}

	*out_boundingSphere = CalculateBoundingSphere(vertices, mesh->Positions.size());


	uint *indices = (uint *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(uint), mesh->Indices.size() / 3);
	memcpy(indices, &mesh->Indices[0], mesh->Indices.size() * sizeof(uint));

	rtcSetGeometryVertexAttributeCount(geometry, 1 + (mesh->TexCoords.size() > 0 ? 1 : 0));

	float3 *normals = (float3 *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, sizeof(float3), mesh->Normals.size());
	memcpy(normals, &mesh->Normals[0], mesh->Normals.size() * sizeof(float3));

	*out_hasNormals = true;

	if (mesh->TexCoords.size() > 0) {
		float2 *texCoords = (float2 *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2, sizeof(float2), mesh->TexCoords.size());
		memcpy(texCoords, &mesh->TexCoords[0], mesh->TexCoords.size() * sizeof(float2));

		*out_hasTexCoords = true;
	} else {
		*out_hasTexCoords = false;
	}

	rtcCommitGeometry(geometry);
	uint meshId = rtcAttachGeometry(m_scene, geometry);
	rtcReleaseGeometry(geometry);

	return meshId;
}

uint Scene::AddLMF(LanternModelFile *lmf, float4x4 &transform, float *out_surfaceArea, float4 *out_boundingSphere, bool *out_hasNormals, bool *out_hasTexCoords) {
	RTCGeometry geometry;
	if (lmf->VerticesPerPrimative == 3) {
		geometry = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_TRIANGLE);
		rtcSetGeometryBuildQuality(geometry, RTC_BUILD_QUALITY_HIGH);
		rtcSetGeometryTimeStepCount(geometry, 1);
	} else if (lmf->VerticesPerPrimative == 4) {
		geometry = rtcNewGeometry(m_device, RTC_GEOMETRY_TYPE_QUAD);
		rtcSetGeometryBuildQuality(geometry, RTC_BUILD_QUALITY_HIGH);
		rtcSetGeometryTimeStepCount(geometry, 1);
	} else {
		printf("Lantern only supports 3 or 4 vertices per primitive. Given [%hhu]\n", lmf->VerticesPerPrimative);
		return -1;
	}

	float3a *vertices = (float3a *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(float3a), lmf->Positions.size() / 3);
	for (uint i = 0, j = 0; j < lmf->Positions.size() / 3; i += 3, ++j) {
		float3a vertex(lmf->Positions[i], lmf->Positions[i + 1], lmf->Positions[i + 2], 1.0f);

		vertices[j] = transform * vertex;
	}

	// Calculate the surface area
	*out_surfaceArea = 0.0f;
	if (lmf->VerticesPerPrimative == 3) {
		for (std::size_t i = 0; i < lmf->Indices.size(); i += 3) {
			float3a v0 = vertices[lmf->Indices[i]];
			float3a v1 = vertices[lmf->Indices[i + 1]];
			float3a v2 = vertices[lmf->Indices[i + 2]];

			// Calculate the area of a triangle using the half cross product: https://math.stackexchange.com/a/128999
			*out_surfaceArea += 0.5f * length(cross(v0 - v1, v0 - v2));
		}

		*out_boundingSphere = CalculateBoundingSphere(vertices, lmf->Positions.size() / 3);

		uint *indices = (uint *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(uint), lmf->Indices.size() / 3);
		memcpy(indices, &lmf->Indices[0], lmf->Indices.size() * sizeof(uint));
	} else if (lmf->VerticesPerPrimative == 4) {
		for (std::size_t i = 0; i < lmf->Indices.size(); i += 4) {
			float3a v0 = vertices[lmf->Indices[i]];
			float3a v1 = vertices[lmf->Indices[i + 1]];
			float3a v2 = vertices[lmf->Indices[i + 2]];
			float3a v3 = vertices[lmf->Indices[i + 2]];

			// Shoelace formula: https://en.wikipedia.org/wiki/Shoelace_formula
			*out_surfaceArea += 0.5f * length(cross(v0 - v1, v0 - v2)) + 0.5f * length(cross(v0 - v2, v0 - v3));
		}

		*out_boundingSphere = CalculateBoundingSphere(vertices, lmf->Positions.size());

		uint *indices = (uint *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT4, 4 * sizeof(uint), lmf->Indices.size() / 4);
		memcpy(indices, &lmf->Indices[0], lmf->Indices.size() * sizeof(uint));
	}

	rtcSetGeometryVertexAttributeCount(geometry, (lmf->Normals.size() > 0 ? 1 : 0) + (lmf->TexCoords.size() > 0 ? 1 : 0));

	if (lmf->Normals.size() > 0) {
		float3 *normals = (float3 *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, sizeof(float3), lmf->Normals.size() / 3);
		memcpy(normals, &lmf->Normals[0], lmf->Normals.size() * sizeof(float));

		*out_hasNormals = true;
	} else {
		*out_hasNormals = false;
	}

	if (lmf->TexCoords.size() > 0) {
		float2 *texCoords = (float2 *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2, sizeof(float2), lmf->TexCoords.size() / 2);
		memcpy(texCoords, &lmf->TexCoords[0], lmf->TexCoords.size() * sizeof(float));

		*out_hasTexCoords = true;
	} else {
		*out_hasTexCoords = false;
	}

	rtcCommitGeometry(geometry);
	uint meshId = rtcAttachGeometry(m_scene, geometry);
	rtcReleaseGeometry(geometry);

	return meshId;
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
	for (auto &light : m_lights) {
		delete light;
	}
	m_lights.clear();

	m_materials.clear();
	m_models.clear();

	m_imageCache.Clear();

	rtcReleaseScene(m_scene);
}

} // End of namespace Lantern
