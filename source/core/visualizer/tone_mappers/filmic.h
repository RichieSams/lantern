/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"
#include "math/vector_math.h"

#include "camera/frame_buffer.h"


namespace Lantern {

inline void TonemapFilmic(FrameBuffer *frameBuffer, byte *dest, float exposure) {
	for (uint j = 0; j < frameBuffer->Height; ++j) {
		const size_t offset = j * frameBuffer->Width;
		for (uint i = 0; i < frameBuffer->Width; ++i) {
			const size_t frameBufferIndex = offset + i;

			float3 color = frameBuffer->ColorData[frameBufferIndex];
			uint colorSampleCount = frameBuffer->ColorSampleCount[frameBufferIndex];

			color = exposure * (color / float(colorSampleCount));
			
			// Color adjustment
			color = max(float3(0.0f), color - float3(0.004f));
			color = (color * (6.2f * color + float3(0.5f))) / (color * (6.2f * color + float3(1.7f)) + float3(0.06f));
			color = clamp(color, 0.0f, 1.0f);

			const size_t destIndex = frameBufferIndex * 4;
			dest[destIndex + 0] = (byte)(color.x * 255.0f); // Red
			dest[destIndex + 1] = (byte)(color.y * 255.0f); // Green
			dest[destIndex + 2] = (byte)(color.z * 255.0f); // Blue
			dest[destIndex + 3] = (byte)255; // Alpha
		}
	}
}

} // End of namespace Lantern
