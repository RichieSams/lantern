/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "bsdfs/bsdf.h"

#include "renderer/surface_interaction.h"

#include "math/uniform_sampler.h"
#include "math/float_math.h"


namespace Lantern {

class MirrorBSDF : public BSDF {
public:
	MirrorBSDF(float3 albedo)
		: BSDF(albedo) {
	}

public:
	float3 Eval(SurfaceInteraction &interaction) const override {
		return m_albedo;
	}

	void Sample(SurfaceInteraction &interaction, UniformSampler *sampler) const override {
		interaction.SampledLobe = BSDFLobe::SpecularReflection;

		interaction.InputDirection = reflect(interaction.OutputDirection, interaction.Normal);
	}

	float Pdf(SurfaceInteraction &interaction) const override {
		float3a reflection = reflect(interaction.InputDirection, interaction.Normal);
		return Float3aNearlyEqual(reflection, interaction.OutputDirection) ? 1.0f : 0.0f;
	}
};

} // End of namespace Lantern
