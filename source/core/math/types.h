/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#if defined(ISPC)

typedef float<2> float2;
typedef float<3> float3;
typedef float<4> float4;

inline varying float dot(const varying float3 &a, const varying float3 &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline varying float length2(const varying float3 &a) {
	return dot(a, a);
}

inline varying float length(const varying float3 &a) {
	return sqrt(dot(a, a));
}

inline varying float3 cross(const varying float3 &a, const varying float3 &b) {
	varying float3 ret = {a.y * b.z - a.z * b.y,
	                      a.z * b.x - a.x * b.z,
	                      a.x * b.y - a.y * b.x};
	return ret;
}

inline varying float3 normalize(const varying float3 &v) {
	return v * (1.f / sqrt(dot(v, v)));
}

struct float3x3 {
	float3 x, y, z;
};

struct float4x4 {
	float4 x, y, z, w;
};

inline varying float3 mul(const varying float3x3 &a, const varying float3 &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline varying float4 mul(const varying float4x4 &a, const varying float4 &b) {
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline varying float3x3 adjugate(const varying float3x3 &a) {
	varying float3x3 ret = {
	    {a.y.y * a.z.z - a.z.y * a.y.z, a.z.y * a.x.z - a.x.y * a.z.z, a.x.y * a.y.z - a.y.y * a.x.z},
	    {a.y.z * a.z.x - a.z.z * a.y.x, a.z.z * a.x.x - a.x.z * a.z.x, a.x.z * a.y.x - a.y.z * a.x.x},
	    {a.y.x * a.z.y - a.z.x * a.y.y, a.z.x * a.x.y - a.x.x * a.z.y, a.x.x * a.y.y - a.y.x * a.x.y}};
	return ret;
}

inline varying float4x4 adjugate(const varying float4x4 &a) {
	varying float4x4 ret = {
	    {a.y.y * a.z.z * a.w.w + a.w.y * a.y.z * a.z.w + a.z.y * a.w.z * a.y.w - a.y.y * a.w.z * a.z.w - a.z.y * a.y.z * a.w.w - a.w.y * a.z.z * a.y.w,
	     a.x.y * a.w.z * a.z.w + a.z.y * a.x.z * a.w.w + a.w.y * a.z.z * a.x.w - a.w.y * a.x.z * a.z.w - a.z.y * a.w.z * a.x.w - a.x.y * a.z.z * a.w.w,
	     a.x.y * a.y.z * a.w.w + a.w.y * a.x.z * a.y.w + a.y.y * a.w.z * a.x.w - a.x.y * a.w.z * a.y.w - a.y.y * a.x.z * a.w.w - a.w.y * a.y.z * a.x.w,
	     a.x.y * a.z.z * a.y.w + a.y.y * a.x.z * a.z.w + a.z.y * a.y.z * a.x.w - a.x.y * a.y.z * a.z.w - a.z.y * a.x.z * a.y.w - a.y.y * a.z.z * a.x.w},
	    {a.y.z * a.w.w * a.z.x + a.z.z * a.y.w * a.w.x + a.w.z * a.z.w * a.y.x - a.y.z * a.z.w * a.w.x - a.w.z * a.y.w * a.z.x - a.z.z * a.w.w * a.y.x,
	     a.x.z * a.z.w * a.w.x + a.w.z * a.x.w * a.z.x + a.z.z * a.w.w * a.x.x - a.x.z * a.w.w * a.z.x - a.z.z * a.x.w * a.w.x - a.w.z * a.z.w * a.x.x,
	     a.x.z * a.w.w * a.y.x + a.y.z * a.x.w * a.w.x + a.w.z * a.y.w * a.x.x - a.x.z * a.y.w * a.w.x - a.w.z * a.x.w * a.y.x - a.y.z * a.w.w * a.x.x,
	     a.x.z * a.y.w * a.z.x + a.z.z * a.x.w * a.y.x + a.y.z * a.z.w * a.x.x - a.x.z * a.z.w * a.y.x - a.y.z * a.x.w * a.z.x - a.z.z * a.y.w * a.x.x},
	    {a.y.w * a.z.x * a.w.y + a.w.w * a.y.x * a.z.y + a.z.w * a.w.x * a.y.y - a.y.w * a.w.x * a.z.y - a.z.w * a.y.x * a.w.y - a.w.w * a.z.x * a.y.y,
	     a.x.w * a.w.x * a.z.y + a.z.w * a.x.x * a.w.y + a.w.w * a.z.x * a.x.y - a.x.w * a.z.x * a.w.y - a.w.w * a.x.x * a.z.y - a.z.w * a.w.x * a.x.y,
	     a.x.w * a.y.x * a.w.y + a.w.w * a.x.x * a.y.y + a.y.w * a.w.x * a.x.y - a.x.w * a.w.x * a.y.y - a.y.w * a.x.x * a.w.y - a.w.w * a.y.x * a.x.y,
	     a.x.w * a.z.x * a.y.y + a.y.w * a.x.x * a.z.y + a.z.w * a.y.x * a.x.y - a.x.w * a.y.x * a.z.y - a.z.w * a.x.x * a.y.y - a.y.w * a.z.x * a.x.y},
	    {a.y.x * a.w.y * a.z.z + a.z.x * a.y.y * a.w.z + a.w.x * a.z.y * a.y.z - a.y.x * a.z.y * a.w.z - a.w.x * a.y.y * a.z.z - a.z.x * a.w.y * a.y.z,
	     a.x.x * a.z.y * a.w.z + a.w.x * a.x.y * a.z.z + a.z.x * a.w.y * a.x.z - a.x.x * a.w.y * a.z.z - a.z.x * a.x.y * a.w.z - a.w.x * a.z.y * a.x.z,
	     a.x.x * a.w.y * a.y.z + a.y.x * a.x.y * a.w.z + a.w.x * a.y.y * a.x.z - a.x.x * a.y.y * a.w.z - a.w.x * a.x.y * a.y.z - a.y.x * a.w.y * a.x.z,
	     a.x.x * a.y.y * a.z.z + a.z.x * a.x.y * a.y.z + a.y.x * a.z.y * a.x.z - a.x.x * a.z.y * a.y.z - a.y.x * a.x.y * a.z.z - a.z.x * a.y.y * a.x.z}};
	return ret;
}

inline varying float determinant(const varying float3x3 &a) {
	return a.x.x * (a.y.y * a.z.z - a.z.y * a.y.z) + a.x.y * (a.y.z * a.z.x - a.z.z * a.y.x) + a.x.z * (a.y.x * a.z.y - a.z.x * a.y.y);
}

inline varying float determinant(const varying float4x4 &a) {
	// clang-format off
    return a.x.x*(a.y.y*a.z.z*a.w.w + a.w.y*a.y.z*a.z.w + a.z.y*a.w.z*a.y.w - a.y.y*a.w.z*a.z.w - a.z.y*a.y.z*a.w.w - a.w.y*a.z.z*a.y.w)
         + a.x.y*(a.y.z*a.w.w*a.z.x + a.z.z*a.y.w*a.w.x + a.w.z*a.z.w*a.y.x - a.y.z*a.z.w*a.w.x - a.w.z*a.y.w*a.z.x - a.z.z*a.w.w*a.y.x)
         + a.x.z*(a.y.w*a.z.x*a.w.y + a.w.w*a.y.x*a.z.y + a.z.w*a.w.x*a.y.y - a.y.w*a.w.x*a.z.y - a.z.w*a.y.x*a.w.y - a.w.w*a.z.x*a.y.y)
         + a.x.w*(a.y.x*a.w.y*a.z.z + a.z.x*a.y.y*a.w.z + a.w.x*a.z.y*a.y.z - a.y.x*a.z.y*a.w.z - a.w.x*a.y.y*a.z.z - a.z.x*a.w.y*a.y.z);
	// clang-format on
}

inline varying float3x3 inverse(const varying float3x3 &a) {
	varying float3x3 adj = adjugate(a);
	varying float det = determinant(a);

	varying float3x3 ret = {
	    {adj.x.x / det, adj.x.y / det, adj.x.z / det},
	    {adj.y.x / det, adj.y.y / det, adj.y.z / det},
	    {adj.z.x / det, adj.z.y / det, adj.z.z / det},
	};
	return ret;
}

inline varying float4x4 inverse(const varying float4x4 &a) {
	varying float4x4 adj = adjugate(a);
	varying float det = determinant(a);

	varying float4x4 ret = {
	    {adj.x.x / det, adj.x.y / det, adj.x.z / det, adj.x.w},
	    {adj.y.x / det, adj.y.y / det, adj.y.z / det, adj.y.w},
	    {adj.z.x / det, adj.z.y / det, adj.z.z / det, adj.z.w},
	    {adj.w.x / det, adj.w.y / det, adj.w.z / det, adj.w.w},
	};
	return ret;
}

inline varying float copysignf(const varying float a, const varying float b) {
	return floatbits(((intbits(b) & 0x80000000) |
	                  (intbits(a) & ~0x80000000)));
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
