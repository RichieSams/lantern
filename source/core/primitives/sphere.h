/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "primitives/primitive.h"


namespace Lantern {

class Sphere : public Primitive {
public:
	void Initialize(RTCDevice device, RTCScene scene, float radius, float3 emissiveColor, float radiantPower, float4x4 transform, BSDF *bsdf, Medium *medium);

private:
	float3 m_origin = float3(0.0f);
	float m_radius = 0.0f;

public:
	float3 SampleDirectLighting(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *direction, float *distance, float *pdf) const override;
	float PdfDirectLighting(Scene *scene, SurfaceInteraction &interaction, float3 inputDirection) const override;
	float3 InterpolateNormal(uint primId, float u, float v) override { return float3(0.0f); }
	float2 InterpolateTexCoords(uint primId, float u, float v) override { return float2(0.0f); }
};

} // End of namespace Lantern
