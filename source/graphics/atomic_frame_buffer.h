/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/typedefs.h"

#include "math/vector_types.h"

#include <atomic>

namespace Lantern {

class AtomicFrameBuffer {
public:
	AtomicFrameBuffer(uint width, uint height);
	~AtomicFrameBuffer();

public:
	uint Width;
	uint Height;

private:
	typedef embree::Vec3<std::atomic<float> > float3Atomic;
	static_assert(sizeof(std::atomic<float>) == 4, "std::atomic<float> is not a simple type! This will break a lot of things");

	float3Atomic *m_colorData;
	float *m_weights;

public:
	void SplatPixel(uint x, uint y, float3a &color) {
		uint index = y * Width + x;

		m_colorData[index].x.store(color.x);
		m_colorData[index].y.store(color.y);
		m_colorData[index].z.store(color.z);
	}

	void GetPixel(uint x, uint y, float3 &pixel) const {
		uint index = y * Width + x;

		pixel.x = m_colorData[index].x.load();
		pixel.y = m_colorData[index].y.load();
		pixel.z = m_colorData[index].z.load();
	}
};

} // End of namespace Lantern
