/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include <stddef.h>

namespace lantern {

template <class T, size_t N>
constexpr size_t ArraySize(T (&)[N]) {
	return N;
}

}