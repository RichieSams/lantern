/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "camera/pinhole_camera.h"

#include <atomic>

namespace lantern {

struct FrameData;

class Integrator {
public:
	Integrator(uint32_t width, uint32_t height);

private:
	PinholeCamera m_camera;

public:
	void RenderOneFrame(FrameData *dest);
};

} // namespace lantern
