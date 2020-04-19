/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/bsdf.h"
#include "materials/textures/texture.h"
#include "materials/bsdf_math.h"

#include "math/sampling.h"


namespace Lantern {

class LambertReflection : public BxDF {
public:
	LambertReflection(Texture *albedo)
		: BxDF(BSDFLobe::Type(BSDFLobe::Reflection | BSDFLobe::Diffuse), albedo) {}

public:
	float3 Eval(float3 outputDirection, float3 inputDirection, float2 texCoord) const override {
		return m_albedo->Sample(texCoord) * kPi;
	}

	float3 Sample(UniformSampler *sampler, float3 outputDirection, float3 *inputDirection, float *pdf, float IORi, float *IORo, float2 texCoord) const override {
		*inputDirection = CosineSampleHemisphere(sampler);
		*pdf = Pdf(outputDirection, *inputDirection, texCoord);
		return Eval(outputDirection, *inputDirection, texCoord);
	}

	float Pdf(float3 outputDirection, float3 inputDirection, float2 texCoord) const override {
		return SameHemisphere(outputDirection, inputDirection) ? AbsCosTheta(inputDirection) * kInvPi : 0.0f;
	}
};

} // End of namespace Lantern
