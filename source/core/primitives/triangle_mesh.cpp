/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "primitives/triangle_mesh.h"

#include "io/lantern_model_file.h"


namespace Lantern {

void TriangleMesh::Initialize(RTCDevice device, RTCScene scene, LanternModelFile *lmf, float3 emissiveColor, float radiantPower, float4x4 transform, BSDF *bsdf, Medium *medium) {
	m_scene = scene;

	RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetGeometryBuildQuality(geometry, RTC_BUILD_QUALITY_HIGH);
	rtcSetGeometryTimeStepCount(geometry, 1);

	float4 *vertices = (float4 *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(float4), lmf->Positions.size() / 3);
	for (uint i = 0, j = 0; j < lmf->Positions.size() / 3; i += 3, ++j) {
		float4 vertex(lmf->Positions[i], lmf->Positions[i + 1], lmf->Positions[i + 2], 1.0f);

		vertices[j] = transform * vertex;
	}

	// Calculate the surface area
	float surfaceArea = 0.0f;
	for (std::size_t i = 0; i < lmf->Indices.size(); i += 3) {
		float4 v0 = vertices[lmf->Indices[i]];
		float4 v1 = vertices[lmf->Indices[i + 1]];
		float4 v2 = vertices[lmf->Indices[i + 2]];

		// Calculate the area of a triangle using the half cross product: https://math.stackexchange.com/a/128999
		surfaceArea += 0.5f * length(cross(v0.xyz() - v1.xyz(), v0.xyz() - v2.xyz()));
	}

	uint *indices = (uint *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(uint), lmf->Indices.size() / 3);
	memcpy(indices, &lmf->Indices[0], lmf->Indices.size() * sizeof(uint));

	rtcSetGeometryVertexAttributeCount(geometry, (lmf->Normals.size() > 0 ? 1 : 0) + (lmf->TexCoords.size() > 0 ? 1 : 0));

	bool hasNormals = false;
	if (lmf->Normals.size() > 0) {
		float3 *normals = (float3 *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, RTC_FORMAT_FLOAT3, sizeof(float3), lmf->Normals.size() / 3);
		memcpy(normals, &lmf->Normals[0], lmf->Normals.size() * sizeof(float));

		hasNormals = true;
	}

	bool hasTexCoords = false;
	if (lmf->TexCoords.size() > 0) {
		float2 *texCoords = (float2 *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, RTC_FORMAT_FLOAT2, sizeof(float2), lmf->TexCoords.size() / 2);
		memcpy(texCoords, &lmf->TexCoords[0], lmf->TexCoords.size() * sizeof(float));

		hasTexCoords = true;
	}

	rtcCommitGeometry(geometry);
	uint geometryId = rtcAttachGeometry(scene, geometry);
	rtcReleaseGeometry(geometry);

	Primitive::Initialize(emissiveColor * radiantPower / surfaceArea, bsdf, medium, surfaceArea, geometryId, hasNormals, hasTexCoords);
}

float3 TriangleMesh::SampleDirectLighting(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *direction, float *distance, float *pdf) const {
	*pdf = 0.0f;

	return float3(0.0f);
}

float3 TriangleMesh::InterpolateNormal(uint primId, float u, float v) {
	float3 normal;
	rtcInterpolate1(rtcGetGeometry(m_scene, m_geometryId), primId, u, v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 0, &normal.x, nullptr, nullptr, 3);

	return normal;
}

float2 TriangleMesh::InterpolateTexCoords(uint primId, float u, float v) {
	float2 texCoord;
	rtcInterpolate1(rtcGetGeometry(m_scene, m_geometryId), primId, u, v, RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE, 1, &texCoord.x, nullptr, nullptr, 2);

	return texCoord;
}

} // End of namespace Lantern
