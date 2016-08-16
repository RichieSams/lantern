/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#ifdef _WIN32
	#define STRUCT_ALIGN(...) __declspec(align(__VA_ARGS__))
#else
	#define STRUCT_ALIGN(...) __attribute__((aligned(__VA_ARGS__)))
#endif
