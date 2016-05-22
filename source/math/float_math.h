/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include <algorithm>


namespace Lantern {

#define EPSILON 0.001f

inline bool FloatNearlyEqual(float a, float b) {
	return std::fabs(a - b) <= EPSILON * std::fmaxf(std::fmaxf(1.0f, std::fabs(a)), std::fabs(b));
}

} // End of namespace Lantern
