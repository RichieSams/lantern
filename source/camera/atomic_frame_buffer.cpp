/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "camera/atomic_frame_buffer.h"


namespace Lantern {

AtomicFrameBuffer::AtomicFrameBuffer(uint width, uint height)
		: Width(width),
		  Height(height) {
	m_colorData = new float3Atomic[width * height];
}

AtomicFrameBuffer::~AtomicFrameBuffer() {
	delete[] m_colorData;
}

} // End of namespace Lantern
