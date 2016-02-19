/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"


namespace Lantern {

class UniformSampler;

class Material {
public:
	Material() {}
	virtual ~Material() {}

public:
	virtual float3 EmissiveColor() = 0;
	virtual float3 Eval(float3a wi, float3a wo, float3a normal) = 0;
	virtual float3a Sample(float3a normal, UniformSampler *sampler, float *pdf) = 0;
};

} // End of namespace Lantern
