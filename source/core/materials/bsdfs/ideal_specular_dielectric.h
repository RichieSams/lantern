/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/bsdfs/bsdf.h"

#include "renderer/surface_interaction.h"

#include "math/uniform_sampler.h"
#include <math/vector_math.h>


namespace Lantern {

class IdealSpecularDielectric : public BSDF {
public:
	IdealSpecularDielectric(float3 albedo, float ior)
		: BSDF(BSDFLobe::Specular, albedo), 
		  m_ior(ior) {
	}

private:
	float m_ior;

public:
	float3 Eval(SurfaceInteraction &interaction) const override {
		return m_albedo;
	}

	void Sample(SurfaceInteraction &interaction, UniformSampler *sampler) const override {
		float VdotN = dot(interaction.OutputDirection, interaction.Normal);
		float IORo = m_ior;
		if (VdotN < 0.0f) {
			IORo = 1.0f;
			interaction.Normal = -interaction.Normal;
			VdotN = -VdotN;
		}

		float eta = interaction.IORi / IORo;
		float sinSquaredThetaT = SinSquaredThetaT(VdotN, eta);
		float fresnel = Fresnel(interaction.IORi, IORo, VdotN, sinSquaredThetaT);
		
		float rand = sampler->NextFloat();
		if (rand <= fresnel) {
			// Reflect
			interaction.InputDirection = reflect(interaction.OutputDirection, interaction.Normal);
			interaction.SampledLobe = BSDFLobe::SpecularReflection;
			interaction.IORo = interaction.IORi;
		} else {
			// Refract
			interaction.InputDirection = refract(interaction.OutputDirection, interaction.Normal, VdotN, eta, sinSquaredThetaT);
			interaction.SampledLobe = BSDFLobe::SpecularTransmission;
			interaction.IORo = IORo;
		}

		if (AnyNan(interaction.InputDirection)) {
			printf("nan");
		}
	}

	float Pdf(SurfaceInteraction &interaction) const override {
		return 1.0f;
	}
};

} // End of namespace Lantern
