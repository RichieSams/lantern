/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "camera/frame_data.h"

#include <atomic>
#include <stdint.h>

struct Scene;

namespace lantern {

struct PresentationBuffer;

class RenderHost {
public:
	explicit RenderHost(Scene *scene, PresentationBuffer *startingPresentationBuffer, std::atomic<PresentationBuffer *> *swapPresentationBuffer);
	~RenderHost();

public:
	std::atomic<uint64_t> GenerationNumber;

private:
	Scene *m_scene;

	FrameData m_accumulationBuffer;
	PresentationBuffer *m_currentPresentationBuffer;
	std::atomic<PresentationBuffer *> *m_swapPresentationBuffer;

public:
	void Run(std::atomic<bool> *quit);
};

} // End of namespace lantern
