/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "camera/frame_buffer.h"


namespace Lantern {

FrameBuffer::FrameBuffer(uint width, uint height)
	: Empty(true),
      Width(width),
	  Height(height),
      ColorData(new float3[width * height]),
      Bounces(new uint64[width * height]),
      Weights(new float[width * height]) {
	Reset();
}

FrameBuffer::~FrameBuffer() {
	delete[] ColorData;
	delete[] Weights;
	delete[] Bounces;
}

void FrameBuffer::Reset() {
	// We rely on the fact that 0x0000 == 0.0f
	memset(&ColorData[0], 0, Width * Height * sizeof(float3));
	memset(&Bounces[0], 0, Width * Height * sizeof(uint64));
	memset(&Weights[0], 0, Width * Height * sizeof(float));
	Empty = true;
}

} // End of namespace Lantern
