/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "linalg.h"
using namespace linalg::aliases;

#include <math.h>
#include <algorithm>


namespace Lantern {

float3x3 CreateCoordinateFrame(float3 &N);

inline float SinSquaredThetaT(float VdotN, float eta) {
	return eta * eta * (1 - VdotN * VdotN);
}

inline float Fresnel(float IORi, float IORo, float VdotN, float sinSquaredThetaT) {
	// Check for total internal reflection
	if (sinSquaredThetaT > 1.0f) {
		return 1.0f;
	}

	float cosThetaT = sqrtf(1.0f - sinSquaredThetaT);
	float R_perpendicular = (IORi * VdotN - IORo * cosThetaT) / (IORi * VdotN + IORo * cosThetaT);
	float R_parallel = (IORo * VdotN - IORi * cosThetaT) / (IORo * VdotN + IORi * cosThetaT);

	return 0.5f * (R_perpendicular * R_perpendicular + R_parallel * R_parallel);
}

inline bool AnyNan(float3 &a) {
	return std::isnan(a.x) || std::isnan(a.y) || std::isnan(a.z);
}

inline bool AnyNan(float2 &a) {
	return std::isnan(a.x) || std::isnan(a.y);
}

inline bool AnyZero(float3 &a) {
	return a.x == 0.0f || a.y == 0.0f || a.z == 0.0f;
}

inline bool AllZero(float3 &a) {
	return a.x == 0.0f && a.y == 0.0f && a.z == 0.0f;
}

inline bool SameHemisphere(const float3 &w, const float3 &wp) {
	return w.z * wp.z > 0;
}

} // End of namespace Lantern
