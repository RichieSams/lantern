/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "linalg.h"
using namespace linalg::aliases;

#include <string.h>

namespace lantern {

struct FrameData {
	FrameData(uint32_t width, uint32_t height)
	        : Width(width), Height(height),
	          ColorData(new float3[width * height]),
	          AlbedoData(new float3[width * height]),
	          NormalData(new float3[width * height]),
	          SampleCount(new uint32_t[width * height]) {
		Reset();
	}
	~FrameData() {
		delete[] ColorData;
		delete[] AlbedoData;
		delete[] NormalData;
		delete[] SampleCount;
	}

	const uint32_t Width;
	const uint32_t Height;
	// Empty is a time saver for the visualizer
	// So it doesn't have to copy a ton of data when everything is just zero
	bool Empty;

	float3 *ColorData;
	float3 *AlbedoData;
	float3 *NormalData;
	uint32_t *SampleCount;

	void Reset() {
		// We rely on the fact that 0x0000 == 0.0f
		memset(&ColorData[0], 0, Width * Height * sizeof(float3));
		memset(&AlbedoData[0], 0, Width * Height * sizeof(float3));
		memset(&NormalData[0], 0, Width * Height * sizeof(float3));
		memset(&SampleCount[0], 0, Width * Height * sizeof(uint32_t));

		Empty = true;
	}
};

} // namespace lantern
