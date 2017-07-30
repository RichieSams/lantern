/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/bsdfs/bsdf.h"
#include "materials/textures/texture.h"

#include "renderer/surface_interaction.h"

#include "math/uniform_sampler.h"
#include "math/float_math.h"


namespace Lantern {

class MirrorBSDF : public BSDF {
public:
	MirrorBSDF(Texture *albedoTexture)
		: BSDF(BSDFLobe::SpecularReflection, albedoTexture) {
	}

public:
	float3 Eval(SurfaceInteraction &interaction) const override {
		return m_albedoTexture->Sample(interaction.TexCoord);
	}

	void Sample(SurfaceInteraction &interaction, UniformSampler *sampler) const override {
		interaction.InputDirection = reflect(interaction.OutputDirection, interaction.Normal);
		interaction.SampledLobe = BSDFLobe::SpecularReflection;
	}

	float Pdf(SurfaceInteraction &interaction) const override {
		return 1.0f;
	}
};

} // End of namespace Lantern
