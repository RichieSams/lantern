/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "math/vector_math.h"


namespace Lantern {

float3a RotateToWorld(float x, float y, float z, float3a &normal) {
	// Find an axis that is not parallel to normal
	float3a majorAxis;
	if (abs(normal.x) < 0.57735026919f /* 1 / sqrt(3) */) {
		majorAxis = float3a(1, 0, 0);
	} else if (abs(normal.y) < 0.57735026919f /* 1 / sqrt(3) */) {
		majorAxis = float3a(0, 1, 0);
	} else {
		majorAxis = float3a(0, 0, 1);
	}

	// Use majorAxis to create a coordinate system relative to world space
	float3a u = normalize(cross(normal, majorAxis));
	float3a v = cross(normal, u);
	float3a w = normal;


	// Transform from local coordinates to world coordinates
	return u * x +
	       v * y +
	       w * z;
}



} // End of namespace Lantern
