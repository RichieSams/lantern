/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "scene/light.h"


namespace Lantern {

class AreaLight : public Light {
public:
	AreaLight(float3 color, float radiantPower, float area, uint geomId, float4 boundingSphere)
		: Light(color * radiantPower * M_1_PI / area),
		  m_area(area),
		  m_geomId(geomId),
		  m_boundingSphere(boundingSphere) {
	}

private:
	float m_area;
	uint m_geomId;
	float4 m_boundingSphere;

public:
	float3 SampleLi(UniformSampler *sampler, Scene *scene, float3a &surfacePos, float3a &surfaceNormal, float3a *wi, float *pdf) const override;
	float PdfLi(Scene *scene, float3a &surfacePos, float3a &surfaceNormal, float3a &wi) const override;
};

} // End of namespace Lantern
