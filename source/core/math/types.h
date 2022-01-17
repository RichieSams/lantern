/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#if defined(ISPC)

typedef float<2> float2;
typedef float<3> float3;

inline uniform float dot(const uniform float3 &a, const uniform float3 &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}
inline varying float dot(const varying float3 &a, const varying float3 &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline uniform float length2(const uniform float3 &a) {
	return dot(a, a);
}
inline varying float length2(const varying float3 &a) {
	return dot(a, a);
}

inline uniform float length(const uniform float3 &a) {
	return sqrt(dot(a, a));
}
inline varying float length(const varying float3 &a) {
	return sqrt(dot(a, a));
}

inline uniform float3 cross(const uniform float3 &a, const uniform float3 &b) {
	uniform float3 ret = {a.y * b.z - a.z * b.y,
	                      a.z * b.x - a.x * b.z,
	                      a.x * b.y - a.y * b.x};
	return ret;
}
inline varying float3 cross(const varying float3 &a, const varying float3 &b) {
	varying float3 ret = {a.y * b.z - a.z * b.y,
	                      a.z * b.x - a.x * b.z,
	                      a.x * b.y - a.y * b.x};
	return ret;
}

inline uniform float3 normalize(const uniform float3 &v) {
	return v * (1.f / sqrt(dot(v, v)));
}
inline varying float3 normalize(const varying float3 &v) {
	return v * (1.f / sqrt(dot(v, v)));
}

#else

#	if defined(_MSC_VER)
__declspec(align(16)) struct float2 {
	float x;
	float y;
};
__declspec(align(16)) struct float3 {
	float x;
	float y;
	float z;
};
#	else
#		include <stdint.h>

struct float2 {
	float x;
	float y;
} __attribute__((aligned(16)));
struct float3 {
	float x;
	float y;
	float z;
} __attribute__((aligned(16)));
#	endif

typedef unsigned uint;

#endif
