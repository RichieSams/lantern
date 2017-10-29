/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"
#include "math/vector_types.h"

#include <vector>


namespace Lantern {

class FrameBuffer {
public:
	FrameBuffer(uint width, uint height);

public:
	uint Width;
	uint Height;

private:
	std::vector<float3> m_colorData;
	std::vector<float> m_weights;
	
	std::vector<uint64> m_bounces;

public:
	void SplatColor(uint x, uint y, float3 &color) {
		const uint index = y * Width + x;

		m_colorData[index] += color;
		m_weights[index] += 1.0f;
	}

	void SplatBounces(uint x, uint y, uint numBounces) {
		const uint index = y * Width + x;

		m_bounces[index] += numBounces;
	}

	void GetPixel(uint x, uint y, float3 &pixel) const {
		uint index = y * Width + x;

		pixel = m_colorData[index];
	}

	float3 *GetColorData() { return &m_colorData[0]; }
	float *GetWeights() { return &m_weights[0]; }

	void Reset() {
		// We rely on the fact that 0x0000 == 0.0f
		memset(&m_colorData[0], 0, Width * Height * sizeof(float3));
		memset(&m_weights[0], 0, Width * Height * sizeof(float));
	}
};

} // End of namespace Lantern
