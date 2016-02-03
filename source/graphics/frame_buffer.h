/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/typedefs.h"

#include "math/vector_types.h"


namespace Lantern {

class FrameBuffer {
public:
	FrameBuffer(uint width, uint height);
	~FrameBuffer();

public:
	uint Width;
	uint Height;

private:
	float3 *m_colorData;
	float *m_weights;

public:
	void SplatPixel(uint x, uint y, float3a &color) {
		uint index = y * Width + x;

		m_colorData[index].x = color.x;
		m_colorData[index].y = color.y;
		m_colorData[index].z = color.z;
	}

	void GetPixel(uint x, uint y, float3 &pixel) const {
		uint index = y * Width + x;

		pixel.x = m_colorData[index].x;
		pixel.y = m_colorData[index].y;
		pixel.z = m_colorData[index].z;
	}

	float3 *GetColorData() { return m_colorData; }
};

} // End of namespace Lantern
