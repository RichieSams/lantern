/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"


namespace Lantern {

class UniformSampler;

class BSDF {
public:
	BSDF(float3 albedo)
		: m_albedo(albedo) {
	}
	virtual ~BSDF() {
	}

protected:
	float3 m_albedo;

public:
	virtual float3 Eval(float3a &wi, float3a &wo, float3a &normal) const = 0;
	virtual float3a Sample(float3a &wo, float3a &normal, UniformSampler *sampler) const = 0;
	virtual float Pdf(float3a &wi, float3a &wo, float3a &normal) const = 0;
};

} // End of namespace Lantern
