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

inline void TonemapScaledMaxRange(FrameBuffer *frameBuffer, byte *dest, float exposure) {
	float3 maxLinearValue(0.0f);

	for (uint j = 0; j < frameBuffer->Height; ++j) {
		const size_t offset = j * frameBuffer->Width;
		for (uint i = 0; i < frameBuffer->Width; ++i) {
			const size_t frameBufferIndex = offset + i;

			float3 color = frameBuffer->ColorData[frameBufferIndex];
			uint colorSampleCount = frameBuffer->ColorSampleCount[frameBufferIndex];

			color = exposure * (color / float(colorSampleCount));
			maxLinearValue = max(maxLinearValue, color);
		}
	}

	for (uint j = 0; j < frameBuffer->Height; ++j) {
		const size_t offset = j * frameBuffer->Width;
		for (uint i = 0; i < frameBuffer->Width; ++i) {
			const size_t frameBufferIndex = offset + i;

			float3 color = frameBuffer->ColorData[frameBufferIndex];
			uint colorSampleCount = frameBuffer->ColorSampleCount[frameBufferIndex];

			color = exposure * (color / float(colorSampleCount));

			// Color adjustment
			color = color / maxLinearValue;
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
