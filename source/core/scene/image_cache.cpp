#include "scene/image_cache.h"

#include "stb/stb_image.h"

#include <algorithm>


namespace Lantern {

ImageCache::~ImageCache() {
	Clear();
}

uint ImageCache::AddImage(const char *filepath) {
	Image image;
	image.Data = stbi_load(filepath, &image.XSize, &image.YSize, &image.NumChannels, 0);
	
	m_images.push_back(std::move(image));

	return (uint)m_images.size() - 1;
}

float3 ImageCache::SampleImage(uint imageId, float2 texCoord) {
	Image *image = &m_images[imageId];

	// Closest sampling for now because it's simple
	std::size_t row = (uint)std::round(texCoord.x * (float)image->XSize);
	std::size_t column = (uint)std::round(texCoord.y * (float)image->YSize);

	std::size_t index = (row * image->XSize + column) * image->NumChannels;
	switch (image->NumChannels) {
	case 1:
	{
		float value = (float)image->Data[index] / 256.0f;
		return float3(value, value, value);
	}
	case 2:
		return float3((float)image->Data[index] / 256.0f, (float)image->Data[index + 1] / 256.0f, 0.0f);
	case 3:
	case 4:
		return float3((float)image->Data[index] / 256.0f, (float)image->Data[index + 1] / 256.0f, (float)image->Data[index + 2] / 256.0f);
	default:
		// Error. Return NaN
		return float3((float)std::nan(nullptr), (float)std::nan(nullptr), (float)std::nan(nullptr));
		break;
	}
}

void ImageCache::Clear() {
	for (Image &image : m_images) {
		stbi_image_free(image.Data);
	}

	m_images.clear();
}

}
