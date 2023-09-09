/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

extern "C" {
struct FrameData {
	int Width;
	int Height;
	// Empty is a time saver for the visualizer
	// So it doesn't have to copy a ton of data when everything is just zero
	bool Empty;

	float *ColorDataR;
	float *ColorDataG;
	float *ColorDataB;
	float *AlbedoDataR;
	float *AlbedoDataG;
	float *AlbedoDataB;
	float *NormalDataR;
	float *NormalDataG;
	float *NormalDataB;
	unsigned int *SampleCount;
};
} // extern C

#if !defined(ISPC)

void FrameDataInit(FrameData *data, unsigned int width, unsigned int height);
void FrameDataTerm(FrameData *data);
void FrameDataReset(FrameData *data);

#endif
