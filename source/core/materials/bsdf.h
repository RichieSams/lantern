/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include "materials/bsdf_lobe.h"


namespace Lantern {

struct SurfaceInteraction;
class UniformSampler;
class Texture;

class BxDF {
public:
	BxDF(BSDFLobe::Type supportedLobes, Texture *albedo)
		: LobeType(supportedLobes),
		  m_albedo(albedo) {}
	virtual ~BxDF() = default;

public:
	BSDFLobe::Type LobeType;

protected:
	Texture *m_albedo;

public:
	virtual float3 Eval(float3 outputDirection, float3 inputDirection, float2 texCoord) const = 0;
	virtual float3 Sample(UniformSampler *sampler, float3 outputDirection, float3 *inputDirection, float *pdf, float IORi, float *IORo, float2 texCoord) const = 0;
	virtual float Pdf(float3a outputDirection, float3a inputDirection, float2 texCoord) const = 0;
	bool MatchesType(BSDFLobe::Type allowedLobes) const { return (allowedLobes & LobeType) == LobeType; }
};

enum {
	kMaxBxDFLobes = 8,
};

class BSDF {
public:
	BSDF() = default;
	virtual ~BSDF();

private:
	BxDF *m_lobes[kMaxBxDFLobes];
	int m_numLobes;

public:
	bool AddLobe(BxDF* lobe);
	
	float3 Eval(SurfaceInteraction &interaction, float3 inputDirection, BSDFLobe::Type allowedLobes) const;
	float3 Sample(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *inputDirection, float *pdf, BSDFLobe::Type allowedLobes, BSDFLobe::Type *sampledLobe, float IORi, float *IORo) const;
	float Pdf(SurfaceInteraction &interaction, float3 inputDirection, BSDFLobe::Type allowedLobes) const;
};


} // End of namespace Lantern
