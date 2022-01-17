/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "math/types.h"

extern "C" {

struct Ray {
	float3 Origin;
	float3 Direction;
};
} // extern C
