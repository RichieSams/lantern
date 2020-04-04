/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include <math.h>
#include <algorithm>


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

float3x3 CreateCoordinateFrame(float3a &N);

inline float3a reflect(const float3a &V, const float3a &N) {
	return 2.0f * dot(V, N) * N - V;
}

inline float3a refract(const float3a &V, const float3a &N, float VdotN, float eta, float sinSquaredThetaT) {
	return (eta * VdotN - sqrtf(1.0f - sinSquaredThetaT)) * N - eta * V;
}

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

inline bool AnyNan(float3a &a) {
	return std::isnan(a.x) || std::isnan(a.y) || std::isnan(a.z);
}

inline bool AnyNan(float2 &a) {
	return std::isnan(a.x) || std::isnan(a.y);
}

inline float3 clamp(float3 &a, float min, float max) {
	return float3(
		std::max(std::min(a.x, max), min),
		std::max(std::min(a.y, max), min),
		std::max(std::min(a.z, max), min)
	);
}

inline float3 max(float3 &a, float3 &b) {
	return float3(
		std::max(a.x, b.x),
		std::max(a.y, b.y),
		std::max(a.z, b.z)
	);
}

inline bool SameHemisphere(const float3 &w, const float3 &wp) {
	return w.z * wp.z > 0;
}

} // End of namespace Lantern
