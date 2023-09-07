/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "math/types.h"

extern "C" {

struct Sphere {
	float3 Origin;
	float RadiusSquared;
};
}

inline Sphere MakeSphere(float3 origin, float radius) {
	Sphere ret = {
	    origin,
	    radius * radius,
	};
	return ret;
}