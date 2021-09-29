/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "linalg.h"
using namespace linalg::aliases;

namespace lantern {

struct Ray {
public:
	Ray() = default;
	Ray(float3 origin, float3 direction)
	        : Origin(origin),
	          Direction(direction) {
	}

public:
	float3 Origin;
	float3 Direction;
};

} // namespace lantern
