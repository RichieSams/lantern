/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "camera/frame_data.h"
#include "camera/pinhole_camera.h"

#include <atomic>

namespace lantern {

class Integrator {
public:
	Integrator(FrameData *startingFrameData, std::atomic<FrameData *> *swapFrameData);

private:
	FrameData *m_currentFrameData;
	std::atomic<FrameData *> *m_swapFrameData;
	PinholeCamera m_camera;

public:
	void RenderOneFrame();
};

} // namespace lantern
