/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "linalg.h"
using namespace linalg::aliases;


namespace Lantern {

struct SurfaceInteraction {
	float3 Position = float3(0.0f);
	float3 Normal = float3(0.0f);
	float2 TexCoord = float2(0.0f);

	struct {
		float3 Normal;
	} Shading;

	float3 OutputDirection = float3(0.0f);
};

} // End of namespace Lantern
