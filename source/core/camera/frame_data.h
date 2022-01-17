/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "math/types.h"

extern "C" {

struct FrameData {
	int Width;
	int Height;
	// Empty is a time saver for the visualizer
	// So it doesn't have to copy a ton of data when everything is just zero
	bool Empty;

	float3 *ColorData;
	float3 *AlbedoData;
	float3 *NormalData;
	uint *SampleCount;
};

// Only define the init functions for C
#if !defined(ISPC)
#	include <string.h>

inline void FrameDataInit(FrameData *data, uint width, uint height) {
	data->Width = width;
	data->Height = height;
	data->ColorData = new float3[width * height];
	data->AlbedoData = new float3[width * height];
	data->NormalData = new float3[width * height];
	data->SampleCount = new uint[width * height];
}

inline void FrameDataTerm(FrameData *data) {
	delete[] data->ColorData;
	delete[] data->AlbedoData;
	delete[] data->NormalData;
	delete[] data->SampleCount;

	data->Width = 0;
	data->Height = 0;
	data->ColorData = NULL;
	data->AlbedoData = NULL;
	data->NormalData = NULL;
	data->SampleCount = NULL;
}

inline void FrameDataReset(FrameData *data) {
	// We rely on the fact that 0x0000 == 0.0f
	memset(&data->ColorData[0], 0, data->Width * data->Height * sizeof(float3));
	memset(&data->AlbedoData[0], 0, data->Width * data->Height * sizeof(float3));
	memset(&data->NormalData[0], 0, data->Width * data->Height * sizeof(float3));
	memset(&data->SampleCount[0], 0, data->Width * data->Height * sizeof(uint));

	data->Empty = true;
}

#endif

} // extern C
