/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "linalg.h"
using namespace linalg::aliases;

#include <stdint.h>

namespace lantern {

struct PresentationBuffer {
	PresentationBuffer(uint32_t width, uint32_t height)
	        : Width(width), Height(height),
	          ResolvedData(new float[width * height * 3]) {
	}
	~PresentationBuffer() {
		delete[] ResolvedData;
	}

	const uint32_t Width;
	const uint32_t Height;

	float *ResolvedData;
};

} // namespace lantern
