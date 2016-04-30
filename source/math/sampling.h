/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"
#include "math/uniform_sampler.h"
#include "math/vector_math.h"
#include <DirectXMath.h>

namespace Lantern {

inline float PowerHeuristic(uint numf, float fPdf, uint numg, float gPdf) {
	float f = numf * fPdf;
	float g = numg * gPdf;

	return (f * f) / (f * f + g * g);
}

inline void UniformSampleDisc(UniformSampler *sampler, float radius, float *x, float *y) {
	float rand = sampler->NextFloat();
	float r = std::sqrtf(rand) * radius;
	float theta = sampler->NextFloat() * 2.0f * M_PI;

	*x = r * std::cosf(theta);
	*y = r * std::sinf(theta);
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
	float z = std::sqrtf(1.0f - x * x - y * y);

	return normalize(RotateToWorld(x, y, z, normal));
}

} // End of namespace Lantern
