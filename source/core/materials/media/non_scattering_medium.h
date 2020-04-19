/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/media/medium.h"

#include "linalg.h"
using namespace linalg::aliases;


namespace Lantern {

class NonScatteringMedium : public Medium {
public:
	NonScatteringMedium(float3 color, float atDistance)
		: Medium(color, atDistance) {
	}

public:
	float SampleDistance(UniformSampler *sampler, float tFar, float *weight, float *pdf) const override {
		*pdf = 1.0f;
		return tFar;
	}
	
	float3 SampleScatterDirection(UniformSampler *sampler, float3 &wo, float *pdf) const override {
		return wo;
	}
	float ScatterDirectionPdf(float3 &wi, float3 &wo) const override {
		return 1.0f;
	}

	float3 Transmission(float distance) const override {
		return exp(-m_absorptionCoefficient * distance);
	}
};

} // End of namespace Lantern
