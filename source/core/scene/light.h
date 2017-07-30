/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/uniform_sampler.h"


namespace Lantern {

struct SurfaceInteraction;
class Scene;

class Light {
public:
	Light(float3 radiance) 
		: m_radiance(radiance) {
	}
	virtual ~Light() = default;

public:
	float3 m_radiance;

public:
	virtual float3 SampleLi(UniformSampler *sampler, Scene *scene, SurfaceInteraction &interaction, float *pdf) const = 0;
	virtual float PdfLi(Scene *scene, SurfaceInteraction &interaction) const = 0;
	virtual float3 Le() const { return m_radiance; }
};

} // End of namespace Lantern
