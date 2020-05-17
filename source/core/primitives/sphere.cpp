/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "primitives/sphere.h"

#include "integrator/surface_interaction.h"

#include "math/vector_math.h"
#include "math/sampling.h"


namespace Lantern {

void Sphere::Initialize(RTCDevice device, RTCScene scene, float radius, float3 emissiveColor, float radiantPower, float4x4 transform, BSDF *bsdf, Medium *medium) {
	float4 origin = transform * float4(0.0f, 0.0f, 0.0f, 1.0f);
	m_origin = float3(origin.x, origin.y, origin.z);
	m_radius = radius;
	
	RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_SPHERE_POINT);
	rtcSetGeometryBuildQuality(geometry, RTC_BUILD_QUALITY_HIGH);
	rtcSetGeometryTimeStepCount(geometry, 1);

	float4 *vertex = (float4 *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT4, sizeof(float4), 1);
	*vertex = float4(m_origin.x, m_origin.y, m_origin.z, radius);

	rtcCommitGeometry(geometry);
	uint geometryId = rtcAttachGeometry(scene, geometry);
	rtcReleaseGeometry(geometry);

	const float surfaceArea = 4.0f * kPi * m_radius * m_radius;
	Primitive::Initialize(emissiveColor * radiantPower * kInvPi / surfaceArea, bsdf, medium, surfaceArea, geometryId, false, false);
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
