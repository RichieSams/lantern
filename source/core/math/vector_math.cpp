/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "math/vector_math.h"

namespace Lantern {

float3x3 CreateCoordinateFrame(float3 &N) {
	// [Duff et al. 17] Building An Orthonormal Basis, Revisited. JCGT. 2017.
	float sign = copysignf(1.0f, N.z);
	const float a = -1.0f / (sign + N.z);
	const float b = N.x * N.y * a;
	float3 tangent(1.0f + sign * N.x * N.x * a, sign * b, -sign * N.x);
	float3 bitangent(b, sign + N.y * N.y * a, -N.y);

	return float3x3{
		{tangent.x, bitangent.x, N.x},
		{tangent.y, bitangent.y, N.y},
		{tangent.z, bitangent.z, N.z}
	};
}

//float3x3 CreateCoordinateFrame(float3 &N) {
//	// Find an axis that is not parallel to normal
//	float3 majorAxis;
//	if (abs(N.x) < 0.57735026919f /* 1 / sqrt(3) */) {
//		majorAxis = float3(1, 0, 0);
//	}
//	else if (abs(N.y) < 0.57735026919f /* 1 / sqrt(3) */) {
//		majorAxis = float3(0, 1, 0);
//	}
//	else {
//		majorAxis = float3(0, 0, 1);
//	}
//
//	// Use majorAxis to create a coordinate system relative to world space
//	float3 u = normalize(cross(N, majorAxis));
//	float3 v = cross(N, u);
//	float3 w = N;
//
//	return float3x3{{u.x, v.x, w.x},
//	                {u.y, v.y, w.y},
//	                {u.z, v.z, w.z}};
//}

} // End of namespace Lantern
