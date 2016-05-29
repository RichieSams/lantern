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

inline float SinSquaredThetaT(float VdotN, float eta) {
	return eta * eta * (1 - VdotN * VdotN);
}

inline float Fresnel(float IORi, float IORo, float VdotN, float sinSquaredThetaT) {
	// Check for total internal reflection
	if (sinSquaredThetaT > 1.0f) {
		return 1.0f;
	}

	float cosThetaT = std::sqrtf(1.0f - sinSquaredThetaT);
	float R_perpendicular = (IORi * VdotN - IORo * cosThetaT) / (IORi * VdotN + IORo * cosThetaT);
	float R_parallel = (IORo * VdotN - IORi * cosThetaT) / (IORo * VdotN + IORi * cosThetaT);

	return 0.5f * (R_perpendicular * R_perpendicular + R_parallel * R_parallel);
}

} // End of namespace Lantern
