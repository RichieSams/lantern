///* Lantern - A path tracer
//*
//* Lantern is the legal property of Adrian Astley
//* Copyright Adrian Astley 2015 - 2016
//*/
//
//#pragma once
//
//#include "materials/bsdfs/bsdf.h"
//#include "materials/textures/texture.h"
//
//#include "integrator/surface_interaction.h"
//
//#include "math/uniform_sampler.h"
//#include "math/float_math.h"
//
//
//namespace Lantern {
//
//class MirrorBSDF : public BSDF {
//public:
//	MirrorBSDF(Texture *albedoTexture)
//		: BSDF(BSDFLobe::SpecularReflection, albedoTexture) {
//	}
//
//public:
//	float3 Eval(SurfaceInteraction &interaction, SurfaceSample &surfaceSample) const override {
//		return m_albedoTexture->Sample(interaction.TexCoord);
//	}
//
//	void Sample(SurfaceInteraction &interaction, UniformSampler *sampler, SurfaceSample *surfaceSample, float *pdf) const override {
//		surfaceSample->InputDirection = reflect(interaction.OutputDirection, interaction.Normal);
//		surfaceSample->SampledLobe = BSDFLobe::SpecularReflection;
//		*pdf = 1.0f;
//	}
//
//	float Pdf(SurfaceInteraction &interaction, SurfaceSample &surfaceSample) const override {
//		return 0.0f;
//	}
//};
//
//} // End of namespace Lantern
