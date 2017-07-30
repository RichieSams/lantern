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

inline void UniformSampleDisc(UniformSampler *sampler, float radius, float *x, float *y) {
	float rand = sampler->NextFloat();
	float r = sqrtf(rand) * radius;
	float theta = sampler->NextFloat() * 2.0f * (float)M_PI;

	*x = r * cosf(theta);
	*y = r * sinf(theta);
}

/**
* Creates a random direction in the hemisphere defined by the normal, weighted by a cosine lobe
*
* Based on http://www.rorydriscoll.com/2009/01/07/better-sampling/
*
* @param normal     The normal that defines the hemisphere
*
* @param sampler    The sampler to use for internal random number generation
* @return           A cosine weighted random direction in the hemisphere
*/
inline float3a CosineSampleHemisphere(float3a &normal, UniformSampler *sampler) {
	// Create coordinates in the local coordinate system
	float x;
	float y;
	UniformSampleDisc(sampler, 1.0f, &x, &y);

	// Project z up to the unit hemisphere
	float z = sqrtf(1.0f - x * x - y * y);

	return normalize(RotateToWorld(x, y, z, normal));
}

inline float3a UniformSampleHemisphere(float3a &normal, UniformSampler *sampler) {
	float cosPhi = sampler->NextFloat();
	float sinPhi = sqrt(1.0f - cosPhi * cosPhi);
	float theta = 2 * (float)M_PI * sampler->NextFloat();

	float x = sinPhi * sinf(theta);
	float y = cosPhi;
	float z = sinPhi * cosf(theta);

	return normalize(RotateToWorld(x, y, z, normal));
}

inline float3a UniformSampleSphere(UniformSampler *sampler) {
	float cosPhi = 2.0f * sampler->NextFloat() - 1.0f;
	float sinPhi = std::sqrt(1.0f - cosPhi * cosPhi);
	float theta = 2 * (float)M_PI * sampler->NextFloat();

	float x = sinPhi * sinf(theta);
	float y = cosPhi;
	float z = sinPhi * cosf(theta);

	return float3a(x, y, z);
}

} // End of namespace Lantern
