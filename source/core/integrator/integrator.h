/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "frame_data/frame_data.h"

#include <atomic>

namespace lantern {

class Integrator {
public:
	Integrator(FrameData *startingFrameData, std::atomic<FrameData *> *swapFrameData);

private:
	FrameData *m_currentFrameData;
	std::atomic<FrameData *> *m_swapFrameData;

public:
	void RenderOneFrame();
};

} // namespace lantern
