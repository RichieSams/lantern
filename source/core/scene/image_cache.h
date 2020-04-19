/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2017
*/

#pragma once

#include "math/int_types.h"

#include "linalg.h"
using namespace linalg::aliases;

#include <vector>


namespace Lantern {

struct Image {
	byte *Data;
	int XSize;
	int YSize;
	int NumChannels;
};

class ImageCache {
public:
	ImageCache() { }
	~ImageCache();

private:
	std::vector<Image> m_images;

public:
	uint AddImage(const char *filepath);
	float3 SampleImage(uint imageId, float2 texCoord);

	void Clear();
};

}
