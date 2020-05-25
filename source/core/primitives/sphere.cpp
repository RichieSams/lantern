/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "primitives/sphere.h"

#include "integrator/surface_interaction.h"

#include "math/vector_math.h"
#include "math/sampling.h"

#include "scene/mesh_elements.h"


namespace Lantern {

static void Subdivide(Mesh* mesh) {
	// Save a copy of the input geometry
	Mesh inputCopy = *mesh;


	mesh->Positions.resize(0);
	mesh->Indices.resize(0);

	//       v1
	//       *
	//      / \
	//     /   \
	//  m0*-----*m1
	//   / \   / \
	//  /   \ /   \
	// *-----*-----*
	// v0    m2     v2

	size_t numTris = inputCopy.Indices.size() / 3;
	for (size_t i = 0; i < numTris; ++i) {
		float3 v0 = inputCopy.Positions[inputCopy.Indices[i * 3 + 0]];
		float3 v1 = inputCopy.Positions[inputCopy.Indices[i * 3 + 1]];
		float3 v2 = inputCopy.Positions[inputCopy.Indices[i * 3 + 2]];


		// Generate the midpoints
		// For subdivision, we just care about the position component
		// We derive the other float3 components in CreateGeosphere
		float3 m0(
			0.5f * (v0.x + v1.x),
			0.5f * (v0.y + v1.y),
			0.5f * (v0.z + v1.z));

		float3 m1(
			0.5f * (v1.x + v2.x),
			0.5f * (v1.y + v2.y),
			0.5f * (v1.z + v2.z));

		float3 m2(
			0.5f * (v0.x + v2.x),
			0.5f * (v0.y + v2.y),
			0.5f * (v0.z + v2.z));

		// Add new geometry.

		mesh->Positions.push_back(v0); // 0
		mesh->Positions.push_back(v1); // 1
		mesh->Positions.push_back(v2); // 2
		mesh->Positions.push_back(m0); // 3
		mesh->Positions.push_back(m1); // 4
		mesh->Positions.push_back(m2); // 5

		mesh->Indices.push_back(i * 6 + 0);
		mesh->Indices.push_back(i * 6 + 3);
		mesh->Indices.push_back(i * 6 + 5);

		mesh->Indices.push_back(i * 6 + 3);
		mesh->Indices.push_back(i * 6 + 4);
		mesh->Indices.push_back(i * 6 + 5);

		mesh->Indices.push_back(i * 6 + 5);
		mesh->Indices.push_back(i * 6 + 4);
		mesh->Indices.push_back(i * 6 + 2);

		mesh->Indices.push_back(i * 6 + 3);
		mesh->Indices.push_back(i * 6 + 1);
		mesh->Indices.push_back(i * 6 + 4);
	}
}

inline float QuadrantAwareArcTan(float y, float x) {
	float result;

	if (x >= 0.0f) {
		// Quadrant I or IV

		// If x = 0, then atanf(y/x) = +pi/2 if y > 0
		//                atanf(y/x) = -pi/2 if y < 0
		result = atanf(y / x); // in [-pi/2, +pi/2]

		if (result < 0.0f) {
			result += 2.0f * (float)kPi; // in [0, 2*pi).
		}
	}
	else {
		// Quadrant II or III

		result = atanf(y / x) + (float)kPi; // in [0, 2*pi).
	}

	return result;
}

static void CreateGeosphere(float radius, uint numSubdivisions, Mesh* mesh) {
	// Approximate a sphere by tessellating an icosahedron.

	const float X = 0.525731f;
	const float Z = 0.850651f;

	float3 pos[12] = {
		float3(-X, 0.0f, -Z),  float3(X, 0.0f, -Z),
		float3(-X, 0.0f, Z), float3(X, 0.0f, Z),
		float3(0.0f, Z, -X),   float3(0.0f, Z, X),
		float3(0.0f, -Z, -X),  float3(0.0f, -Z, X),
		float3(Z, X, 0.0f),   float3(-Z, X, 0.0f),
		float3(Z, -X, 0.0f),  float3(-Z, -X, 0.0f)
	};

	int k[60] = {
		1,4,0,  4,9,0,  4,5,9,  8,5,4,  1,8,4,
		1,10,8, 10,3,8, 8,3,5,  3,2,5,  3,7,2,
		3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
		10,1,6, 11,0,9, 2,11,9, 5,2,9,  11,2,7
	};

	mesh->Positions.resize(12);
	mesh->Indices.resize(60);
	for (uint i = 0; i < 12; ++i) {
		mesh->Positions[i] = pos[i];
	}
	for (uint i = 0; i < 60; ++i) {
		mesh->Indices[i] = k[i];
	}

	// Tesselate
	for (uint i = 0; i < numSubdivisions; ++i) {
		Subdivide(mesh);
	}

	std::size_t numVertices = mesh->Positions.size();
	mesh->Normals.resize(numVertices);
	mesh->Tangents.clear();
	mesh->TexCoords.clear();

	// Project vertices onto sphere and scale.
	for (uint i = 0; i < numVertices; ++i) {
		// Project onto unit sphere.
		float3 n = normalize(mesh->Positions[i]);
		float3 p = float3(radius * n);

		mesh->Positions[i] = p;
		mesh->Normals[i] = n;
	}
}

void Sphere::Initialize(RTCDevice device, RTCScene scene, float radius, float3 emissiveColor, float radiantPower, float4x4 transform, BSDF *bsdf, Medium *medium) {
	float4 origin = transform * float4(0.0f, 0.0f, 0.0f, 1.0f);
	m_origin = float3(origin.x, origin.y, origin.z);
	m_radius = radius;
	
	RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_TRIANGLE);
	rtcSetGeometryBuildQuality(geometry, RTC_BUILD_QUALITY_HIGH);
	rtcSetGeometryTimeStepCount(geometry, 1);

	Mesh mesh;
	CreateGeosphere(radius, 4, &mesh);
	
	float4* vertices = (float4*)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(float4), mesh.Positions.size());
	for (uint i = 0; i < mesh.Positions.size(); ++i) {
		float4 vertex(mesh.Positions[i], 1.0f);

		vertices[i] = transform * vertex;
	}

	// Calculate the surface area
	float surfaceArea = 0.0f;
	for (std::size_t i = 0; i < mesh.Indices.size(); i += 3) {
		float4 v0 = vertices[mesh.Indices[i]];
		float4 v1 = vertices[mesh.Indices[i + 1]];
		float4 v2 = vertices[mesh.Indices[i + 2]];

		// Calculate the area of a triangle using the half cross product: https://math.stackexchange.com/a/128999
		surfaceArea += 0.5f * length(cross(v0.xyz() - v1.xyz(), v0.xyz() - v2.xyz()));
	}

	uint* indices = (uint*)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_INDEX, 0, RTC_FORMAT_UINT3, 3 * sizeof(uint), mesh.Indices.size() / 3);
	memcpy(indices, &mesh.Indices[0], mesh.Indices.size() * sizeof(uint));

	rtcCommitGeometry(geometry);
	uint geometryId = rtcAttachGeometry(scene, geometry);
	rtcReleaseGeometry(geometry);

	Primitive::Initialize(emissiveColor * radiantPower / surfaceArea, bsdf, medium, surfaceArea, geometryId, false, false);
}

float3 Sphere::SampleDirectLighting(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *direction, float *distance, float *pdf) const {
	// Using the method and notations from PBRT: http://www.pbr-book.org/3ed-2018/Light_Transport_I_Surface_Reflection/Sampling_Light_Sources.html#fragment-SphereMethodDefinitions-5
	float distanceSquared = distance2(interaction.Position, m_origin);

	// If we're inside the sphere, just uniformly sample from the sphere
	if (distanceSquared <= m_radius * m_radius) {
		float3 localPosition = m_radius * UniformSampleUnitSphere(sampler);
		float3 globalPosition = m_origin + localPosition;
		
		float3 L = globalPosition - interaction.Position;

		*direction = normalize(L);
		*distance = length(L);
		*pdf = UniformUnitSpherePdf() / (m_radius * m_radius);
		
		return m_emission;
	}

	// Otherwise, sample the sphere uniformly inside the subtended cone

	float2 rand = sampler->NextFloat2();
	float distanceToOrigin = std::sqrtf(distanceSquared);

	// Compute theta and phi values for sample in cone
	float sinThetaMax = m_radius / distanceToOrigin;
	float sinThetaMaxSquared = sinThetaMax * sinThetaMax;
	float invSinThetaMax = 1 / sinThetaMax;
	float cosThetaMax = std::sqrtf(std::max(0.0f, 1.0f - sinThetaMaxSquared));

	float cosTheta = (cosThetaMax - 1.0f) * rand.x + 1.0f;
	float sinThetaSquared = 1.0f - cosTheta * cosTheta;

	if (sinThetaMaxSquared < 0.00068523f /* sin^2(1.5 deg) */) {
		// Fall back to a Taylor series expansion for small angles, where
		// the standard approach suffers from severe cancellation errors
		sinThetaSquared = sinThetaMaxSquared * rand.x;
		cosTheta = std::sqrtf(1.0f - sinThetaSquared);
	}

	// Compute angle alpha from center of sphere to sampled point on surface
	float cosAlpha = sinThetaSquared * invSinThetaMax + cosTheta * std::sqrtf(std::max(0.0f, 1.0f - sinThetaSquared * invSinThetaMax * invSinThetaMax));
	float sinAlpha = std::sqrtf(std::max(0.0f, 1.0f - cosAlpha * cosAlpha));

	// Phi is just a random point in 2 Pi
	float phi = rand.y * k2Pi;

	float3 localPosition = m_radius * float3(std::cosf(phi) * sinAlpha, std::sin(phi) * sinAlpha, cosAlpha);
	float3 normalizedDirectionToOrigin = normalize(m_origin - interaction.Position);
	float3x3 frame = CreateCoordinateFrame(normalizedDirectionToOrigin);
	float3x3 inverseFrame = inverse(frame);
	
	float3 globalPosition = inverseFrame * localPosition;

	float3 L = globalPosition - interaction.Position;

	*direction = normalize(L);
	*distance = length(L);
	*pdf = 1.0f / (k2Pi * (1.0f - sinThetaMax));

	return m_emission;
}

float Sphere::PdfDirectLighting(Scene *scene, SurfaceInteraction &interaction, float3 inputDirection) const {
	float distanceSquared = distance2(interaction.Position, m_origin);

	// Return uniform PDF if point is inside sphere
	if (distanceSquared <= m_radius * m_radius) {
		return Primitive::PdfDirectLighting(scene, interaction, inputDirection);
	}

	// Compute general sphere pdf
	float sinThetaMaxSquared = m_radius * m_radius / distanceSquared;
	float cosThetaMax = std::sqrtf(std::max(0.0f, 1.0f - sinThetaMaxSquared));
	return 1.0f / (k2Pi * (1.0f - cosThetaMax));
}

} // End of namespace Lantern
