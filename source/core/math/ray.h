/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "linalg.h"
using namespace linalg::aliases;

namespace lantern {

class Ray {
public:
	Ray() = default;
	Ray(float3 origin, float3 direction)
	        : m_origin(origin),
	          m_direction(direction) {
	}

private:
	float3 m_origin;
	float3 m_direction;
};

} // namespace lantern
