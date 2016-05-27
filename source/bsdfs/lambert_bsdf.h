/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "bsdfs/bsdf.h"

#include "renderer/surface_interaction.h"

#include "math/sampling.h"


namespace Lantern {

class LambertBSDF : public BSDF {
public:
	LambertBSDF(float3 albedo)
		: BSDF(albedo) {
	}

public:
	float3 Eval(SurfaceInteraction &interaction) const override {
		return m_albedo * M_1_PI * dot(interaction.InputDirection, interaction.Normal);
	}
	
	void Sample(SurfaceInteraction &interaction, UniformSampler *sampler) const override {
		interaction.SampledLobe = BSDFLobe::Diffuse;

		interaction.InputDirection = CosineSampleHemisphere(interaction.Normal, sampler);
	}

	float Pdf(SurfaceInteraction &interaction) const override {
		return dot(interaction.InputDirection, interaction.Normal) * M_1_PI;
	}
};

} // End of namespace Lantern
