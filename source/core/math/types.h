/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#if defined(ISPC)

#	include "math/types.isph"

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
