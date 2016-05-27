/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"
#include "math/vector_types.h"


namespace Lantern {

class UniformSampler;
struct SurfaceInteraction;
class BSDF;
class Scene;
class Light;

class Renderer {
public:
	Renderer(Scene *scene)
		: m_scene(scene),
		  m_frameNumber(0u) {
	};

private:
	static const uint kTileSize = 8;

	Scene *m_scene;
	uint m_frameNumber;

public:
	void RenderFrame();

private:
	void RenderTile(uint index, uint width, uint height, uint numTilesX, uint numTilesY) const;
	void RenderPixel(uint x, uint y, UniformSampler *sampler) const;
	float3 SampleOneLight(UniformSampler *sampler, SurfaceInteraction interaction, BSDF *bsdf, Light *hitLight) const;
	float3 EstimateDirect(Light *light, UniformSampler *sampler, SurfaceInteraction &interaction, BSDF *bsdf) const;
};

} // End of namespace Lantern
