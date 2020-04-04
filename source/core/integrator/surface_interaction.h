/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"


namespace Lantern {

struct SurfaceInteraction {
	float3a Position = float3a(0.0f);
	float3a Normal = float3a(0.0f);
	float2 TexCoord = float2(0.0f);

	struct {
		float3a Normal;
	} Shading;

	float3 OutputDirection = float3(0.0f);
};

} // End of namespace Lantern
