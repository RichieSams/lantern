/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/typedefs.h"


namespace Lantern {

class UniformSampler;
class Scene;

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

};

} // End of namespace Lantern
