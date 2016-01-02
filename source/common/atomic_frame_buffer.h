/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/typedefs.h"

#include "common/vector_types.h"

#include <atomic>

namespace Lantern {

class AtomicFrameBuffer {
public:
	AtomicFrameBuffer(uint width, uint height);
	~AtomicFrameBuffer();

private:
	typedef Vec3<std::atomic<float> > float3a;
	static_assert(sizeof(std::atomic<float>) == 4, "std::atomic<float> is not a simple type! This will break a lot of things");

	float3a *m_data;

	uint m_width;
	uint m_height;

public:
	uint Width() const { return m_width; }
	uint Height() const { return m_height; }

	void SplatPixel(uint x, uint y, float3 &color) {
		uint index = y * m_width + x;

		m_data[index].X.store(color.X);
		m_data[index].Y.store(color.Y);
		m_data[index].Y.store(color.Z);
	}

	void GetPixel(uint x, uint y, float3 &pixel) const {
		uint index = y * m_width + x;

		pixel.X = m_data[index].X.load();
		pixel.Y = m_data[index].Y.load();
		pixel.Z = m_data[index].Z.load();
	}
};

} // End of namespace Lantern
