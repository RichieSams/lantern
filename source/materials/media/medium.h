/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include <cmath>


namespace Lantern {
class UniformSampler;

class Medium {
public:
	Medium(float3 absorptionColor, float absorptionAtDistance) 
		: m_absorptionCoefficient(-log(absorptionColor) / absorptionAtDistance) {
		// This method for calculating the absorption coefficient is borrowed from Burley's 2015 Siggraph Course Notes "Extending the Disney BRDF to a BSDF with Integrated Subsurface Scattering"
		// It's much more intutive to specify a color and a distance, then back-calculate the coefficient
	}
	virtual ~Medium() {
	}

protected:
	const float3a m_absorptionCoefficient;

public:
	virtual float SampleDistance(UniformSampler *sampler, float tFar, float *weight, float *pdf) const = 0;

	virtual float3a SampleScatterDirection(UniformSampler *sampler, float3a &wo, float *pdf) const = 0;
	virtual float ScatterDirectionPdf(float3a &wi, float3a &wo) const = 0;

	virtual float3 Transmission(float distance) const = 0;
};

} // End of namespace Lantern
