/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/types.h"

extern "C" {

struct SurfaceInteraction {
	float3 Position;
	float3 Normal;
	float2 TexCoord;

	struct {
		float3 Normal;
	} Shading;

	float3 OutputDirection;
};
} // extern C
