/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#define kPi (3.14159265358979323846f)
#define k2Pi (6.28318530717958647693f)
#define kInvPi (0.31830988618379067154f)
#define kInv2Pi (0.15915494309189533577f)
#define kInv4Pi (0.07957747154594766788f)
#define kPiOver2 (1.57079632679489661923f)
#define kPiOver4 (0.78539816339744830961f)
#define kSqrt2 (1.41421356237309504880f)
#define kPiOver180 (0.01745329251f)

#if defined(ISPC)
#	define kInfinity (floatbits(0x7f800000))
#else
#	include <limits>
static constexpr float kInfinity = std::numeric_limits<float>::infinity();
#endif
