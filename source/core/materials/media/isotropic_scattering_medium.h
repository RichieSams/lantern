/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/media/medium.h"

#include "math/int_types.h"
#include "math/uniform_sampler.h"
#include "math/sampling.h"


namespace Lantern {

class IsotropicScatteringMedium : public Medium {
public:
	IsotropicScatteringMedium(float3 absorptionColor, float absorptionAtDistance, float scatteringDistance)
		: Medium(absorptionColor, absorptionAtDistance),
		  m_scatteringCoefficient(1 / scatteringDistance) {
	}

private:
	float m_scatteringCoefficient;

public:
	float SampleDistance(UniformSampler *sampler, float tFar, float *weight, float *pdf) const override {
		float distance = -logf(sampler->NextFloat()) / m_scatteringCoefficient;
		if (distance >= tFar) {
			*pdf = 1.0f;
			return tFar;
		}

		*pdf = std::exp(-m_scatteringCoefficient * distance);
		return distance;
	}
	float3 SampleScatterDirection(UniformSampler *sampler, float3 &wo, float *pdf) const override {
		*pdf = kInv4Pi;
		return UniformSampleUnitSphere(sampler);
	}

	float ScatterDirectionPdf(float3 &wi, float3 &wo) const override {
		return kInv4Pi;
	}
	float3 Transmission(float distance) const override {
		return exp(-m_absorptionCoefficient * distance);
	}
};

} // End of namespace Lantern
