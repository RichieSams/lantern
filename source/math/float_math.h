/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include <algorithm>


namespace Lantern {

#define EPSILON 0.001f

inline bool FloatNearlyEqual(float a, float b) {
	return std::fabs(a - b) <= EPSILON * std::fmaxf(std::fmaxf(1.0f, std::fabs(a)), std::fabs(b));
}

inline bool Float3NearlyEqual(float3 a, float3 b) {
	return FloatNearlyEqual(a.x, b.x) && FloatNearlyEqual(a.y, b.y) && FloatNearlyEqual(a.z, b.z);
}

inline bool Float3aNearlyEqual(float3a a, float3a b) {
	return FloatNearlyEqual(a.x, b.x) && FloatNearlyEqual(a.y, b.y) && FloatNearlyEqual(a.z, b.z);
}

} // End of namespace Lantern
