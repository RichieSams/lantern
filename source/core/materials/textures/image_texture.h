/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2017
*/

#pragma once

#include "materials/textures/texture.h"

#include "math/int_types.h"

#include "scene/image_cache.h"


namespace Lantern {

	class ImageTexture : public Texture {
	public:
		ImageTexture(ImageCache *imageCache, uint imageId)
			: m_imageCache(imageCache),
			  m_imageId(imageId) {
		}
		~ImageTexture() { }

	private:
		ImageCache *m_imageCache;
		uint m_imageId;


	public:
		float3 Sample(float2 texCoord) override {
			// UV assume texCoords start in the bottom left
			// Textures assume texCoords start in the top left
			// So we have to invert texCoord.y to fix this discrepancy
			texCoord.y = 1.0f - texCoord.y;

			return m_imageCache->SampleImage(m_imageId, texCoord);
		}
	};

} // End of namespace Lantern
