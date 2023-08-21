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

struct float3x3 {
	float3 x, y, z;
};

struct float4x4 {
	float4 x, y, z, w;
};

inline uniform float3 mul(const uniform float3x3 &a, const uniform float3 &b) {
	uniform float3 tempA = {a.x.x * b.x, a.x.y * b.x, a.x.z * b.x};
	uniform float3 tempB = {a.y.x * b.y, a.y.y * b.y, a.y.z * b.y};
	uniform float3 tempC = {a.z.x * b.z, a.z.y * b.z, a.z.z * b.z};

	uniform float3 ret = {tempA.x + tempB.x + tempC.x, tempA.y + tempB.y + tempC.y, tempA.z + tempB.z + tempC.z};
	return ret;
}
inline varying float3 mul(const varying float3x3 &a, const varying float3 &b) {
	varying float3 tempA = {a.x.x * b.x, a.x.y * b.x, a.x.z * b.x};
	varying float3 tempB = {a.y.x * b.y, a.y.y * b.y, a.y.z * b.y};
	varying float3 tempC = {a.z.x * b.z, a.z.y * b.z, a.z.z * b.z};

	varying float3 ret = {tempA.x + tempB.x + tempC.x, tempA.y + tempB.y + tempC.y, tempA.z + tempB.z + tempC.z};
	return ret;
}

inline uniform float4 mul(const uniform float4x4 &a, const uniform float4 &b) {
	uniform float4 tempA = {a.x.x * b.x, a.x.y * b.x, a.x.z * b.x, a.x.w * b.x};
	uniform float4 tempB = {a.y.x * b.y, a.y.y * b.y, a.y.z * b.y, a.y.w * b.y};
	uniform float4 tempC = {a.z.x * b.z, a.z.y * b.z, a.z.z * b.z, a.z.w * b.z};
	uniform float4 tempD = {a.w.x * b.w, a.w.y * b.w, a.w.z * b.w, a.w.w * b.w};

	uniform float4 ret = {tempA.x + tempB.x + tempC.x + tempD.x, tempA.y + tempB.y + tempC.y + tempD.y, tempA.z + tempB.z + tempC.z + tempD.z, tempA.w + tempB.w + tempC.w + tempD.w};
	return ret;
}
inline varying float4 mul(const varying float4x4 &a, const varying float4 &b) {
	varying float4 tempA = {a.x.x * b.x, a.x.y * b.x, a.x.z * b.x, a.x.w * b.x};
	varying float4 tempB = {a.y.x * b.y, a.y.y * b.y, a.y.z * b.y, a.y.w * b.y};
	varying float4 tempC = {a.z.x * b.z, a.z.y * b.z, a.z.z * b.z, a.z.w * b.z};
	varying float4 tempD = {a.w.x * b.w, a.w.y * b.w, a.w.z * b.w, a.w.w * b.w};

	varying float4 ret = {tempA.x + tempB.x + tempC.x + tempD.x, tempA.y + tempB.y + tempC.y + tempD.y, tempA.z + tempB.z + tempC.z + tempD.z, tempA.w + tempB.w + tempC.w + tempD.w};
	return ret;
}

inline uniform float3x3 adjugate(const uniform float3x3 &a) {
	uniform float3x3 ret = {
	    {a.y.y * a.z.z - a.z.y * a.y.z, a.z.y * a.x.z - a.x.y * a.z.z, a.x.y * a.y.z - a.y.y * a.x.z},
	    {a.y.z * a.z.x - a.z.z * a.y.x, a.z.z * a.x.x - a.x.z * a.z.x, a.x.z * a.y.x - a.y.z * a.x.x},
	    {a.y.x * a.z.y - a.z.x * a.y.y, a.z.x * a.x.y - a.x.x * a.z.y, a.x.x * a.y.y - a.y.x * a.x.y}};
	return ret;
}
inline varying float3x3 adjugate(const varying float3x3 &a) {
	varying float3x3 ret = {
	    {a.y.y * a.z.z - a.z.y * a.y.z, a.z.y * a.x.z - a.x.y * a.z.z, a.x.y * a.y.z - a.y.y * a.x.z},
	    {a.y.z * a.z.x - a.z.z * a.y.x, a.z.z * a.x.x - a.x.z * a.z.x, a.x.z * a.y.x - a.y.z * a.x.x},
	    {a.y.x * a.z.y - a.z.x * a.y.y, a.z.x * a.x.y - a.x.x * a.z.y, a.x.x * a.y.y - a.y.x * a.x.y}};
	return ret;
}

inline uniform float4x4 adjugate(const uniform float4x4 &a) {
	uniform float4x4 ret = {
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

inline uniform float determinant(const uniform float3x3 &a) {
	return a.x.x * (a.y.y * a.z.z - a.z.y * a.y.z) + a.x.y * (a.y.z * a.z.x - a.z.z * a.y.x) + a.x.z * (a.y.x * a.z.y - a.z.x * a.y.y);
}
inline varying float determinant(const varying float3x3 &a) {
	return a.x.x * (a.y.y * a.z.z - a.z.y * a.y.z) + a.x.y * (a.y.z * a.z.x - a.z.z * a.y.x) + a.x.z * (a.y.x * a.z.y - a.z.x * a.y.y);
}

inline uniform float determinant(const uniform float4x4 &a) {
	// clang-format off
    return a.x.x*(a.y.y*a.z.z*a.w.w + a.w.y*a.y.z*a.z.w + a.z.y*a.w.z*a.y.w - a.y.y*a.w.z*a.z.w - a.z.y*a.y.z*a.w.w - a.w.y*a.z.z*a.y.w)
         + a.x.y*(a.y.z*a.w.w*a.z.x + a.z.z*a.y.w*a.w.x + a.w.z*a.z.w*a.y.x - a.y.z*a.z.w*a.w.x - a.w.z*a.y.w*a.z.x - a.z.z*a.w.w*a.y.x)
         + a.x.z*(a.y.w*a.z.x*a.w.y + a.w.w*a.y.x*a.z.y + a.z.w*a.w.x*a.y.y - a.y.w*a.w.x*a.z.y - a.z.w*a.y.x*a.w.y - a.w.w*a.z.x*a.y.y)
         + a.x.w*(a.y.x*a.w.y*a.z.z + a.z.x*a.y.y*a.w.z + a.w.x*a.z.y*a.y.z - a.y.x*a.z.y*a.w.z - a.w.x*a.y.y*a.z.z - a.z.x*a.w.y*a.y.z);
	// clang-format on
}
inline varying float determinant(const varying float4x4 &a) {
	// clang-format off
    return a.x.x*(a.y.y*a.z.z*a.w.w + a.w.y*a.y.z*a.z.w + a.z.y*a.w.z*a.y.w - a.y.y*a.w.z*a.z.w - a.z.y*a.y.z*a.w.w - a.w.y*a.z.z*a.y.w)
         + a.x.y*(a.y.z*a.w.w*a.z.x + a.z.z*a.y.w*a.w.x + a.w.z*a.z.w*a.y.x - a.y.z*a.z.w*a.w.x - a.w.z*a.y.w*a.z.x - a.z.z*a.w.w*a.y.x)
         + a.x.z*(a.y.w*a.z.x*a.w.y + a.w.w*a.y.x*a.z.y + a.z.w*a.w.x*a.y.y - a.y.w*a.w.x*a.z.y - a.z.w*a.y.x*a.w.y - a.w.w*a.z.x*a.y.y)
         + a.x.w*(a.y.x*a.w.y*a.z.z + a.z.x*a.y.y*a.w.z + a.w.x*a.z.y*a.y.z - a.y.x*a.z.y*a.w.z - a.w.x*a.y.y*a.z.z - a.z.x*a.w.y*a.y.z);
	// clang-format on
}

inline uniform float3x3 inverse(const uniform float3x3 &a) {
	uniform float3x3 adj = adjugate(a);
	uniform float det = determinant(a);

	uniform float3x3 ret = {
	    {adj.x.x / det, adj.x.y / det, adj.x.z / det},
	    {adj.y.x / det, adj.y.y / det, adj.y.z / det},
	    {adj.z.x / det, adj.z.y / det, adj.z.z / det},
	};
	return ret;
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

inline uniform float4x4 inverse(const uniform float4x4 &a) {
	uniform float4x4 adj = adjugate(a);
	uniform float det = determinant(a);

	uniform float4x4 ret = {
	    {adj.x.x / det, adj.x.y / det, adj.x.z / det, adj.x.w},
	    {adj.y.x / det, adj.y.y / det, adj.y.z / det, adj.y.w},
	    {adj.z.x / det, adj.z.y / det, adj.z.z / det, adj.z.w},
	    {adj.w.x / det, adj.w.y / det, adj.w.z / det, adj.w.w},
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

inline uniform float copysignf(const uniform float a, const uniform float b) {
	return floatbits(((intbits(b) & 0x80000000) |
	                  (intbits(a) & ~0x80000000)));
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
