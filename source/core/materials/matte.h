/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/bsdf.h"
#include "materials/bxdfs/lambert.h"
#include "integrator/surface_interaction.h"

namespace Lantern {

//
//class MatteMaterial : public BSDF {
//public:
//	MatteMaterial(Texture *albedo)
//		: m_lambertReflection(albedo) {}
//
//private:
//	LambertReflection m_lambertReflection;
//
//public:
//	float3 Eval(SurfaceInteraction &interaction, float3 inputDirection, BSDFLobe::Type allowedLobes) const override {
//
//		float3x3 frame = CreateCoordinateFrame(interaction.Shading.Normal);
//		float3x3 inverseFrame = frame.inverse();
//
//		float3 localInputDirection = inverseFrame * inputDirection;
//		float3 localOutputDirection = inverseFrame * interaction.OutputDirection;
//
//		if (localOutputDirection.z == 0.0f) {
//			return float3(0.0f);
//		}
//
//		float3 f(0.0f);
//		const bool reflect = dot(interaction.OutputDirection, interaction.Normal) * dot(inputDirection, interaction.Normal) > 0;
//		if (m_lambertReflection.LobesSupported(allowedLobes) && reflect && (m_lambertReflection.SupportedLobes & BSDFLobe::Reflection)) {
//			f += m_lambertReflection.Eval(localOutputDirection, localInputDirection, interaction.TexCoord);
//		}
//
//		return f;
//	}
//	float3 Sample(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *inputDirection, float *pdf, BSDFLobe::Type allowedLobes, BSDFLobe::Type *sampledLobe, float IORi, float *IORo) const override {
//		
//	}
//	float Pdf(SurfaceInteraction &interaction, float3 inputDirection) const override;
//};

} // End of namespace Lantern
