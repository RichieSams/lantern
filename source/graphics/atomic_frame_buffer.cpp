/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "graphics/atomic_frame_buffer.h"


namespace Lantern {

AtomicFrameBuffer::AtomicFrameBuffer(uint width, uint height)
		: m_width(width),
		  m_height(height) {
	m_data = new float3a[width * height * 3];
}

AtomicFrameBuffer::~AtomicFrameBuffer() {
	delete[] m_data;
}

} // End of namespace Lantern
