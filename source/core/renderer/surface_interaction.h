/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include "materials/bsdfs/bsdf_lobe.h"


namespace Lantern {

struct SurfaceInteraction {
	SurfaceInteraction()
		: SampledLobe(BSDFLobe::Null), 
		  IORi(0.0f), 
		  IORo(0.0f) {
	}

	float3a Position;
	float3a Normal;
	float2 TexCoord;
	float3a InputDirection;
	float3a OutputDirection;
	BSDFLobe::Type SampledLobe;
	float IORi;
	float IORo;
};

} // End of namespace Lantern
