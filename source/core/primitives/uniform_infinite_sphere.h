/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "primitives/primitive.h"


namespace Lantern {

class UniformInfiniteSphere : public Primitive {
private:
	RTCScene m_scene = nullptr;

public:
	void Initialize(float3 emissiveColor, float radiance);

public:
	float3 SampleDirectLighting(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *direction, float *distance, float *pdf) const override;
	float3 InterpolateNormal(uint primId, float u, float v) override { return float3(0.0f); }
	float2 InterpolateTexCoords(uint primId, float u, float v) override { return float2(0.0f); }
};

} // End of namespace Lantern
