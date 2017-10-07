/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2017
*/

#pragma once

#include "materials/textures/texture.h"


namespace Lantern {

	class UVTexture : public Texture {
	public:
		float3 Sample(float2 texCoord) override {
			return float3(texCoord.x, texCoord.y, 0.0f);
		}
	};

} // End of namespace Lantern
