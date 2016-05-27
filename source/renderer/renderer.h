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
	void RenderTile(uint index, uint width, uint height, uint numTilesX, uint numTilesY);
	void RenderPixel(uint x, uint y, UniformSampler *sampler);
	float3 SampleOneLight(UniformSampler *sampler, float3a &surfacePos, float3a &surfaceNormal, float3a &wo, BSDF *material, Light *hitLight) const;
	float3 EstimateDirect(Light *light, UniformSampler *sampler, float3a &surfacePos, float3a &surfaceNormal, float3a &wo, BSDF *material) const;
};

} // End of namespace Lantern
