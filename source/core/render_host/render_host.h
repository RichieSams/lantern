/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "camera/frame_data.h"

#include <atomic>
#include <stdint.h>

namespace lantern {

class Integrator;
struct PresentationBuffer;

class RenderHost {
public:
	explicit RenderHost(Integrator *integrator, PresentationBuffer *startingPresentationBuffer, std::atomic<PresentationBuffer *> *swapPresentationBuffer);

public:
	std::atomic<uint64_t> GenerationNumber;

private:
	Integrator *m_integrator;

	FrameData m_accumulationBuffer;
	PresentationBuffer *m_currentPresentationBuffer;
	std::atomic<PresentationBuffer *> *m_swapPresentationBuffer;

public:
	void Run(std::atomic<bool> *quit);
};

} // End of namespace lantern
