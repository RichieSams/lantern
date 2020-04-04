/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "primitives/primitive.h"


namespace Lantern {

class TriangleMesh : public Primitive {
private:
	RTCScene m_scene = nullptr;

public:
	void Initialize(RTCDevice device, RTCScene scene, LanternModelFile *lmf, float3 emissiveColor, float radiantPower, float4x4 transform, BSDF *bsdf, Medium *medium);

public:
	float3 SampleDirectLighting(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *direction, float *distance, float *pdf) const override;
	float3 InterpolateNormal(uint primId, float u, float v) override;
	float2 InterpolateTexCoords(uint primId, float u, float v) override;
};

} // End of namespace Lantern
