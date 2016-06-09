/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include "materials/media/medium.h"

#include <cmath>


namespace Lantern {

class NonScatteringMedium : public Medium {
public:
	NonScatteringMedium(float3 color, float atDistance)
		: Medium(color, atDistance, float3(0.0f), 0.0f) {
	}

public:
	float SampleDistance(UniformSampler *sampler, float tFar, float *weight) const override {
		return tFar;
	}
	
	float3a SampleScatterDirection(UniformSampler *sampler, float3a &wo, float *pdf) const override {
		return wo;
	}
	float ScatterDirectionPdf(float3a &wi, float3a &wo) const override {
		return 1.0f;
	}

	float3 Transmission(float distance) const override {
		return exp(-m_absorptionCoefficient * distance);
	}
};

} // End of namespace Lantern
