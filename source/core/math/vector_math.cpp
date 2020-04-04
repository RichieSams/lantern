/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "math/vector_math.h"


namespace Lantern {

float3x3 CreateCoordinateFrame(float3a &N) {
	// [Duff et al. 17] Building An Orthonormal Basis, Revisited. JCGT. 2017.
	float sign = copysignf(1.0f, N.z);
	const float a = -1.0f / (sign + N.z);
	const float b = N.x * N.y * a;
	float3 tangent(1.0f + sign * N.x * N.x * a, sign * b, -sign * N.x);
	float3 bitangent(b, sign + N.y * N.y * a, -N.y);

	return float3x3(tangent, bitangent, N);
}

} // End of namespace Lantern
