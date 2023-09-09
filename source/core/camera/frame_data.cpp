/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "camera/frame_data.h"

#include <string.h>

void FrameDataInit(FrameData *data, unsigned int width, unsigned int height) {
	data->Width = width;
	data->Height = height;
	data->ColorDataR = new float[width * height];
	data->ColorDataG = new float[width * height];
	data->ColorDataB = new float[width * height];
	data->AlbedoDataR = new float[width * height];
	data->AlbedoDataG = new float[width * height];
	data->AlbedoDataB = new float[width * height];
	data->NormalDataR = new float[width * height];
	data->NormalDataG = new float[width * height];
	data->NormalDataB = new float[width * height];
	data->SampleCount = new unsigned int[width * height];
}

void FrameDataTerm(FrameData *data) {
	delete[] data->ColorDataR;
	delete[] data->ColorDataG;
	delete[] data->ColorDataB;
	delete[] data->AlbedoDataR;
	delete[] data->AlbedoDataG;
	delete[] data->AlbedoDataB;
	delete[] data->NormalDataR;
	delete[] data->NormalDataG;
	delete[] data->NormalDataB;
	delete[] data->SampleCount;

	data->Width = 0;
	data->Height = 0;
	data->ColorDataR = NULL;
	data->ColorDataG = NULL;
	data->ColorDataB = NULL;
	data->AlbedoDataR = NULL;
	data->AlbedoDataG = NULL;
	data->AlbedoDataB = NULL;
	data->NormalDataR = NULL;
	data->NormalDataG = NULL;
	data->NormalDataB = NULL;
	data->SampleCount = NULL;
}

void FrameDataReset(FrameData *data) {
	// We rely on the fact that 0x0000 == 0.0f
	memset(&data->ColorDataR[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->ColorDataG[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->ColorDataB[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->AlbedoDataR[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->AlbedoDataG[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->AlbedoDataB[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->NormalDataR[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->NormalDataG[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->NormalDataB[0], 0, data->Width * data->Height * sizeof(float));
	memset(&data->SampleCount[0], 0, data->Width * data->Height * sizeof(unsigned int));

	data->Empty = true;
}
