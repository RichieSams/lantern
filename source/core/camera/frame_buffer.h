/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"

#include "linalg.h"
using namespace linalg::aliases;

#include <vector>


namespace Lantern {

class FrameBuffer {
public:
	FrameBuffer(uint width, uint height);
	~FrameBuffer();

public:
	bool Empty;

	uint Width;
	uint Height;

	float3 *ColorData;
	uint *Bounces;
	uint *ColorSampleCount;

public:
	void Reset();
};

} // End of namespace Lantern
