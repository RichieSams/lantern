/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "math/constants.h"
#include "math/ray.h"

extern "C" {
struct PinholeCamera {
	int Width;
	int Height;
	float TanFOVXDiv2;
	float TanFOVYDiv2;
};
} // extern C

#if !defined(ISPC)

#	include <math.h>

inline void PinholeCameraInit(PinholeCamera *camera, int width, int height, float horizontalFOV) {
	camera->Width = width;
	camera->Height = height;
	camera->TanFOVXDiv2 = tanf(horizontalFOV * kPiOver180 * 0.5f);
	camera->TanFOVYDiv2 = tanf(horizontalFOV / (float)width * (float)height * kPiOver180 * 0.5f);
}

#endif
