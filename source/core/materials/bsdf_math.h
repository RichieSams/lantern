/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include <algorithm>

template <typename T, typename U, typename V>
inline T Clamp(T val, U low, V high) {
	if (val < low)
		return low;
	else if (val > high)
		return high;
	else
		return val;
}

inline float CosTheta(const float3 &w) { return w.z; }
inline float Cos2Theta(const float3 &w) { return w.z * w.z; }
inline float AbsCosTheta(const float3 &w) { return abs(w.z); }
inline float Sin2Theta(const float3 &w) {
	return std::max((float)0, (float)1 - Cos2Theta(w));
}

inline float SinTheta(const float3 &w) { return sqrtf(Sin2Theta(w)); }

inline float TanTheta(const float3 &w) { return SinTheta(w) / CosTheta(w); }

inline float Tan2Theta(const float3 &w) {
	return Sin2Theta(w) / Cos2Theta(w);
}

inline float CosPhi(const float3 &w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 1 : Clamp(w.x / sinTheta, -1, 1);
}

inline float SinPhi(const float3 &w) {
	float sinTheta = SinTheta(w);
	return (sinTheta == 0) ? 0 : Clamp(w.y / sinTheta, -1, 1);
}

inline float Cos2Phi(const float3 &w) { return CosPhi(w) * CosPhi(w); }

inline float Sin2Phi(const float3 &w) { return SinPhi(w) * SinPhi(w); }

inline float CosDPhi(const float3 &wa, const float3 &wb) {
	return Clamp(
		(wa.x * wb.x + wa.y * wb.y) / sqrt((wa.x * wa.x + wa.y * wa.y) *
		(wb.x * wb.x + wb.y * wb.y)),
		-1, 1);
}

inline bool SameHemisphere(const float3 &w, const float3 &wp) {
	return w.z * wp.z > 0;
}
