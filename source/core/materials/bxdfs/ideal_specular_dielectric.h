///* Lantern - A path tracer
//*
//* Lantern is the legal property of Adrian Astley
//* Copyright Adrian Astley 2015 - 2016
//*/
//
//#pragma once
//
//#include "materials/bsdf.h"
//#include "materials/textures/texture.h"
//
//#include "integrator/surface_interaction.h"
//#include "integrator/surface_sample.h"
//
//#include "math/uniform_sampler.h"
//#include <math/vector_math.h>
//
//
//namespace Lantern {
//
//class IdealSpecularDielectric : public BSDF {
//public:
//	IdealSpecularDielectric(Texture *albedoTexture, float ior)
//		: BSDF(BSDFLobe::Specular, albedoTexture), 
//		  m_ior(ior) {
//	}
//
//private:
//	float m_ior;
//
//public:
//	float3 Eval(SurfaceInteraction &interaction, float3a inputDirection) const override {
//		return m_albedoTexture->Sample(interaction.TexCoord);
//	}
//
//	float3 Sample(UniformSampler *sampler, SurfaceInteraction &interaction, float3a *inputDirection, float *pdf, BSDFLobe::Type allowedLobes, BSDFLobe::Type *sampledLobe, float IORi, float *IORo) const override {
//		float VdotN = dot(interaction.OutputDirection, interaction.Normal);
//		float IORo = m_ior;
//		if (VdotN < 0.0f) {
//			IORo = 1.0f;
//			interaction.Normal = -interaction.Normal;
//			VdotN = -VdotN;
//		}
//
//		float eta = interaction.IORi / IORo;
//		float sinSquaredThetaT = SinSquaredThetaT(VdotN, eta);
//		float fresnel = Fresnel(interaction.IORi, IORo, VdotN, sinSquaredThetaT);
//		
//		float rand = sampler->NextFloat();
//		if (rand <= fresnel) {
//			// Reflect
//			surfaceSample->InputDirection = reflect(interaction.OutputDirection, interaction.Normal);
//			surfaceSample->SampledLobe = BSDFLobe::SpecularReflection;
//			surfaceSample->IORo = interaction.IORi;
//		} else {
//			// Refract
//			surfaceSample->InputDirection = refract(interaction.OutputDirection, interaction.Normal, VdotN, eta, sinSquaredThetaT);
//			surfaceSample->SampledLobe = BSDFLobe::SpecularTransmission;
//			surfaceSample->IORo = IORo;
//		}
//
//		*pdf = 1.0f;
//
//		if (AnyNan(surfaceSample->InputDirection)) {
//			printf("nan");
//		}
//	}
//
//	float Pdf(SurfaceInteraction &interaction, float3a inputDirection) const override {
//		return 0.0f;
//	}
//};
//
//} // End of namespace Lantern
