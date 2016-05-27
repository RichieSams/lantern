/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"


namespace Lantern {

struct SurfaceInteraction;
class UniformSampler;

class BSDF {
public:
	BSDF(float3 albedo)
		: m_albedo(albedo) {
	}
	virtual ~BSDF() {
	}

protected:
	float3 m_albedo;

public:
	virtual float3 Eval(SurfaceInteraction &interaction) const = 0;
	virtual void Sample(SurfaceInteraction &interaction, UniformSampler *sampler) const = 0;
	virtual float Pdf(SurfaceInteraction &interaction) const = 0;
};

} // End of namespace Lantern
