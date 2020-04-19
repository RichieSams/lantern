/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "scene/scene.h"

#include "materials/bsdf.h"
#include "materials/textures/constant_texture.h"
#include "materials/textures/image_texture.h"
#include "materials/textures/uv_texture.h"
#include "materials/bxdfs/lambert.h"
#include "materials/media/isotropic_scattering_medium.h"
#include "materials/media/non_scattering_medium.h"

#include "io/lantern_model_file.h"

#include "primitives/grid.h"
#include "primitives/quad_mesh.h"
#include "primitives/sphere.h"
#include "primitives/triangle_mesh.h"


namespace Lantern {

void Scene::ParseCamera(nlohmann::json &root) {
	nlohmann::json& camera = root["camera"];

	float3 position = float3(camera["position"][0].get<float>(), camera["position"][1].get<float>(), camera["position"][2].get<float>());
	float3 target = float3(camera["target"][0].get<float>(), camera["target"][1].get<float>(), camera["target"][2].get<float>());
	float3 up = float3(camera["up"][0].get<float>(), camera["up"][1].get<float>(), camera["up"][2].get<float>());
	uint clientWidth = camera["client_width"].get<uint>();
	uint clientHeight = camera["client_height"].get<uint>();

	float fov = kInv4Pi;
	if (camera.count("fov") == 1) {
		fov = camera["fov"].get<float>();
	}

	Camera = new PinholeCamera(position, target, up, clientWidth, clientHeight, fov);
}

void Scene::ParseMaterials(nlohmann::json &root, std::unordered_map<std::string, BSDF *> * bsdfMap) {
	if (root.count("materials") == 0) {
		return;
	}

	for (auto& material : root["materials"]) {
		std::string name = material["name"].get<std::string>();
		BSDF* bsdf = new BSDF();
		
		std::string type = material["type"].get<std::string>();
		if (type == "matte") {
			Texture* albedo = ParseTexture(material["albedo"]);
			m_textures.push_back(albedo);

			BxDF* bxdf = new LambertReflection(albedo);
			bsdf->AddLobe(bxdf);
		}

		m_bsdfs.push_back(bsdf);
		(*bsdfMap)[name] = bsdf;
	}
}

void Scene::ParseMedia(nlohmann::json &root, std::unordered_map<std::string, Medium *> *mediaMap) {
	if (root.count("media") == 0) {
		return;
	}
	
	for (auto& medium : root["media"]) {
		std::string name = medium["name"].get<std::string>();
		std::string type = medium["type"].get<std::string>();
		if (type == "non_scattering") {
			Medium* newMedia = new NonScatteringMedium(
				float3(medium["absorption_color"][0].get<float>(), medium["absorption_color"][1].get<float>(), medium["absorption_color"][2].get<float>()),
				medium["absorption_at_distance"].get<float>());
			m_media.push_back(newMedia);
			(*mediaMap)[name] = newMedia;
		}
		else if (type == "isotropic_scattering") {
			Medium* newMedia = new IsotropicScatteringMedium(
				float3(medium["absorption_color"][0].get<float>(), medium["absorption_color"][1].get<float>(), medium["absorption_color"][2].get<float>()),
				medium["absorption_at_distance"].get<float>(),
				medium["scattering_distance"].get<float>());
			m_media.push_back(newMedia);
			(*mediaMap)[name] = newMedia;
		}
	}
}

bool Scene::ParsePrimitives(nlohmann::json &root, std::unordered_map<std::string, BSDF *> &bsdfMap, std::unordered_map<std::string, Medium *> &mediaMap) {
	if (root.count("primitives") == 0) {
		return true;
	}
	
	for (auto &primitive : root["primitives"]) {
		std::string name = primitive["name"].get<std::string>();

		float4x4 transform = linalg::identity;
		float3 emissiveColor = float3(0.0f);
		float radiantPower = 0.0f;
		Medium* medium = nullptr;

		// Required parameters
		const std::string materialStr = primitive["material"].get<std::string>();
		auto materialIter = bsdfMap.find(materialStr);
		if (materialIter == bsdfMap.end()) {
			printf("Failed to find material `%s` for primitive `%s`\n", materialStr.c_str(), name.c_str());
			return false;
		}
		BSDF* bsdf = materialIter->second;


		// Optional parameters
		if (primitive.count("transform") == 1) {
			nlohmann::json t = primitive["transform"];
			transform = float4x4{
				{t[0].get<float>(), t[4].get<float>(),  t[8].get<float>(), t[12].get<float>()},
				{t[1].get<float>(), t[5].get<float>(),  t[9].get<float>(), t[13].get<float>()},
				{t[2].get<float>(), t[6].get<float>(), t[10].get<float>(), t[14].get<float>()},
				{t[3].get<float>(), t[7].get<float>(), t[11].get<float>(), t[15].get<float>()}};
		}

		if (primitive.count("emission") == 1) {
			emissiveColor = float3(primitive["emission"]["color"][0].get<float>(), primitive["emission"]["color"][1].get<float>(), primitive["emission"]["color"][2].get<float>());
			radiantPower = primitive["emission"]["radiant_power"].get<float>();
		}

		if (primitive.count("medium") == 1) {
			medium = mediaMap[primitive["medium"].get<std::string>()];
		}


		// Initialize the primitive based on the type
		std::string type = primitive["type"].get<std::string>();
		if (type == "lmf") {
			std::string lmfFilePathString = primitive["file_path"].get<std::string>();
			std::filesystem::path lmfFilePath(lmfFilePathString);
			if (lmfFilePath.is_relative()) {
				lmfFilePath = m_jsonPath.parent_path() / lmfFilePath;
			}

			FILE* file = fopen(lmfFilePath.u8string().c_str(), "rb");
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

			switch (lmf.VerticesPerPrimitive) {
			case 3:
			{
				TriangleMesh* primitive = new TriangleMesh();
				primitive->Initialize(m_device, m_scene, &lmf, emissiveColor, radiantPower, transform, bsdf, medium);
				m_primitives[primitive->m_geometryId] = primitive;
				break;
			}
			case 4:
			{
				QuadMesh* primitive = new QuadMesh();
				primitive->Initialize(m_device, m_scene, &lmf, emissiveColor, radiantPower, transform, bsdf, medium);

				m_primitives[primitive->m_geometryId] = primitive;
				break;
			}
			default:
				printf("Invalid LMF file. Supported VerticesPerPrimitive: [3, 4]. Given: %c", lmf.VerticesPerPrimitive);
				return false;
			}
		}
		else if (type == "grid") {
			float width = primitive["width"].get<float>();
			float depth = primitive["depth"].get<float>();

			Grid* primitive = new Grid();
			primitive->Initialize(m_device, m_scene, width, depth, emissiveColor, radiantPower, transform, bsdf, medium);
			m_primitives[primitive->m_geometryId] = primitive;
		}
		else if (type == "sphere") {
			float radius = primitive["radius"].get<float>();

			Sphere* primitive = new Sphere();
			primitive->Initialize(m_device, m_scene, radius, emissiveColor, radiantPower, transform, bsdf, medium);
			m_primitives[primitive->m_geometryId] = primitive;
		}
		else {
			printf("Unknown primitive type: [%s]\n", type.c_str());
			continue;
		}
	}

	for (auto& keyValue : m_primitives) {
		if (keyValue.second->IsEmissive()) {
			m_lights.push_back(keyValue.second);
		}
	}

	return true;
}

Texture *Scene::ParseTexture(nlohmann::json &root) {
	if (root["type"] == "constant") {
		return new ConstantTexture(float3(root["value"][0].get<float>(), root["value"][1].get<float>(), root["value"][2].get<float>()));
	}
	else if (root["type"] == "image") {
		uint imageId = m_imageCache.AddImage(root["file_path"].get<std::string>().c_str());
		return new ImageTexture(&m_imageCache, imageId);
	}
	else if (root["type"] == "uv") {
		return new UVTexture();
	}
}

} // End of namespace Lantern