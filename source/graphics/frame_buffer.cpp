/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "graphics/frame_buffer.h"


namespace Lantern {

FrameBuffer::FrameBuffer(uint width, uint height)
	: Width(width),
	Height(height) {
	m_colorData = new float3[width * height];
}

FrameBuffer::~FrameBuffer() {
	delete[] m_colorData;
}

} // End of namespace Lantern
