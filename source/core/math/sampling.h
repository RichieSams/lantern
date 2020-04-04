/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"
#include "math/uniform_sampler.h"
#include "math/vector_math.h"


namespace Lantern {

inline float PowerHeuristic(uint numf, float fPdf, uint numg, float gPdf) {
	float f = numf * fPdf;
	float g = numg * gPdf;

	return (f * f) / (f * f + g * g);
}

inline float PowerHeuristic(uint numf, float fPdf, uint numg, float gPdf, uint numh, float hPdf) {
	float f = numf * fPdf;
	float g = numg * gPdf;
	float h = numh * hPdf;

	return (f * f) / (f * f + g * g + h * h);
}

inline float2 UniformSampleDisc(UniformSampler *sampler) {
	float r = sqrt(sampler->NextFloat());
	float theta = 2.0f * (float)M_PI * sampler->NextFloat();
	return float2(r * cosf(theta), r * sinf(theta));
}

inline float2 ConcentricSampleDisk(UniformSampler *sampler) {
	float2 rand = sampler->NextFloat2();

	// Map uniform random numbers to $[-1,1]^2$
	float2 uOffset = 2.0f * rand - 1.0f;

	// Handle degeneracy at the origin
	if (uOffset.x == 0.0f && uOffset.y == 0.0f) {
		return float2(0.0f, 0.0f);
	}

	// Apply concentric mapping to point
	float theta;
	float r;
	if (abs(uOffset.x) > abs(uOffset.y)) {
		r = uOffset.x;
		theta = (float)M_PI_4 * (uOffset.y / uOffset.x);
	} else {
		r = uOffset.y;
		theta = (float)M_PI_2 - (float)M_PI_4 * (uOffset.x / uOffset.y);
	}

	return r * float2(cosf(theta), sinf(theta));
}

/**
* Creates a random direction in the unit hemisphere, weighted by a cosine lobe
* 
* @param sampler    The sampler to use for internal random number generation
* @return           A cosine weighted random direction in the hemisphere
*/
inline float3a CosineSampleHemisphere(UniformSampler *sampler) {
	// Create coordinates in the local coordinate system
	float2 d = ConcentricSampleDisk(sampler);

	// Project z up to the unit hemisphere
	float z = sqrtf(std::max(0.0f, 1.0f - d.x * d.x - d.y * d.y));

	return normalize(float3(d.x, d.y, z));
}

inline float3a UniformSampleHemisphere(float3a &normal, UniformSampler *sampler) {
	float cosPhi = sampler->NextFloat();
	float sinPhi = sqrt(1.0f - cosPhi * cosPhi);
	float theta = 2 * (float)M_PI * sampler->NextFloat();

	float x = sinPhi * sinf(theta);
	float y = cosPhi;
	float z = sinPhi * cosf(theta);

	float3x3 frame = CreateCoordinateFrame(normal);
	return normalize(frame * float3(x, y, z));
}

inline float UniformHemispherePdf() {
	return 1.0f / (2.0f * (float)M_PI);
}

inline float3a UniformSampleUnitSphere(UniformSampler *sampler) {
	float cosPhi = 2.0f * sampler->NextFloat() - 1.0f;
	float sinPhi = std::sqrt(1.0f - cosPhi * cosPhi);
	float theta = 2 * (float)M_PI * sampler->NextFloat();

	float x = sinPhi * sinf(theta);
	float y = cosPhi;
	float z = sinPhi * cosf(theta);

	return float3a(x, y, z);
}

inline float UniformUnitSpherePdf() {
	return 1.0f / (4.0f * (float)M_PI);
}

inline float3a UniformSampleSphericalCap(float3a &normal, UniformSampler *sampler, float cosThetaMax) {
	float2 rand = sampler->NextFloat2();

	float cosTheta = (1.0f - rand.x) + rand.x * cosThetaMax;
	float sinTheta = std::sqrtf(1.0f - cosTheta * cosTheta);
	float phi = rand.y * (float)M_2_PI;

	float3x3 frame = CreateCoordinateFrame(normal);
	return normalize(frame * float3(std::cosf(phi) * sinTheta, std::sin(phi) * sinTheta, cosTheta));
}

inline float UniformSphericalCapPdf(float cosThetaMax) {
	return 1.0f / ((float)M_2_PI * (1.0f - cosThetaMax));
}

} // End of namespace Lantern
