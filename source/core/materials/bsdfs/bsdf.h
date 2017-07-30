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
class Texture;

class BSDF {
public:
	BSDF(BSDFLobe::Type supportedLobes, Texture *albedoTexture)
		: SupportedLobes(supportedLobes),
		  m_albedoTexture(albedoTexture) {
	}
	virtual ~BSDF() = default;

public:
	BSDFLobe::Type SupportedLobes;

protected:
	Texture *m_albedoTexture;

public:
	virtual float3 Eval(SurfaceInteraction &interaction) const = 0;
	virtual void Sample(SurfaceInteraction &interaction, UniformSampler *sampler) const = 0;
	virtual float Pdf(SurfaceInteraction &interaction) const = 0;
};

} // End of namespace Lantern
