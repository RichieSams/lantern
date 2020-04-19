/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "linalg.h"
using namespace linalg::aliases;


namespace Lantern {
class UniformSampler;

class Medium {
public:
	Medium(float3 absorptionColor, float absorptionAtDistance) 
		: m_absorptionCoefficient(-log(absorptionColor) / absorptionAtDistance) {
		// This method for calculating the absorption coefficient is borrowed from Burley's 2015 Siggraph Course Notes "Extending the Disney BRDF to a BSDF with Integrated Subsurface Scattering"
		// It's much more intutive to specify a color and a distance, then back-calculate the coefficient
	}
	virtual ~Medium() = default;

protected:
	const float3 m_absorptionCoefficient;

public:
	virtual float SampleDistance(UniformSampler *sampler, float tFar, float *weight, float *pdf) const = 0;

	virtual float3 SampleScatterDirection(UniformSampler *sampler, float3 &wo, float *pdf) const = 0;
	virtual float ScatterDirectionPdf(float3 &wi, float3 &wo) const = 0;

	virtual float3 Transmission(float distance) const = 0;
};

} // End of namespace Lantern
