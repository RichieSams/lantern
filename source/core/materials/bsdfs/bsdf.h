/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include "materials/bsdfs/bsdf_lobe.h"


namespace Lantern {

struct SurfaceInteraction;
class UniformSampler;

class BSDF {
public:
	BSDF(BSDFLobe::Type supportedLobes, float3 albedo)
		: SupportedLobes(supportedLobes),
		  m_albedo(albedo) {
	}
	virtual ~BSDF() {
	}

public:
	BSDFLobe::Type SupportedLobes;

protected:
	float3 m_albedo;

public:
	virtual float3 Eval(SurfaceInteraction &interaction) const = 0;
	virtual void Sample(SurfaceInteraction &interaction, UniformSampler *sampler) const = 0;
	virtual float Pdf(SurfaceInteraction &interaction) const = 0;
};

} // End of namespace Lantern
