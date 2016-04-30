/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"


namespace Lantern {

inline bool all(float3 &a) {
	return a.x == 0.0f && a.y == 0.0f && a.z == 0.0f;
}
inline bool all(float3a &a) {
	return a.x == 0.0f && a.y == 0.0f && a.z == 0.0f;
}

inline bool any(float3 &a) {
	return a.x == 0.0f || a.y == 0.0f || a.z == 0.0f;
}
inline bool any(float3a &a) {
	return a.x == 0.0f || a.y == 0.0f || a.z == 0.0f;
}

float3a RotateToWorld(float x, float y, float z, float3a &normal);

} // End of namespace Lantern
