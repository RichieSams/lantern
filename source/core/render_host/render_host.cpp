/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "render_host/render_host.h"

#include "render_host/presentation_buffer.h"

#include "scene/scene.h"

#include "integrator/integrator.h"

#include <math.h>

namespace lantern {

RenderHost::RenderHost(Scene *scene, PresentationBuffer *startingPresentationBuffer, std::atomic<PresentationBuffer *> *swapPresentationBuffer)
        : GenerationNumber(1),
          m_scene(scene),
          m_currentPresentationBuffer(startingPresentationBuffer), m_swapPresentationBuffer(swapPresentationBuffer) {
	FrameDataInit(&m_accumulationBuffer, startingPresentationBuffer->Width, startingPresentationBuffer->Height);
}
RenderHost::~RenderHost() {
	FrameDataTerm(&m_accumulationBuffer);
}

void RenderHost::Run(std::atomic<bool> *quit) {
	unsigned int frameNumber = 0;

	while (!quit->load(std::memory_order_relaxed)) {
		RenderOneFrame(m_scene, &m_accumulationBuffer, frameNumber++);

		// Now resolve the new data
		for (uint32_t y = 0; y < m_accumulationBuffer.Height; ++y) {
			const uint32_t offset = y * m_accumulationBuffer.Width;
			for (uint32_t x = 0; x < m_accumulationBuffer.Width; ++x) {
				const uint32_t frameBufferIndex = offset + x;
				const uint32_t resolvedDataIndex = frameBufferIndex * 3;

				const uint32_t sampleCount = m_accumulationBuffer.SampleCount[frameBufferIndex];

				// Color data

				// "Gamma-correct" the color data by sqrt() it
				m_currentPresentationBuffer->ResolvedData[resolvedDataIndex + 0] = sqrtf(m_accumulationBuffer.ColorDataR[frameBufferIndex] / sampleCount); // Red
				m_currentPresentationBuffer->ResolvedData[resolvedDataIndex + 1] = sqrtf(m_accumulationBuffer.ColorDataG[frameBufferIndex] / sampleCount); // Green
				m_currentPresentationBuffer->ResolvedData[resolvedDataIndex + 2] = sqrtf(m_accumulationBuffer.ColorDataB[frameBufferIndex] / sampleCount); // Blue

				// TODO: normal and albedo
			}
		}

		// Swap the presentation buffer to hand it off to the viewer
		m_currentPresentationBuffer = std::atomic_exchange(m_swapPresentationBuffer, m_currentPresentationBuffer);

		// Release the data by updating the generation number
		GenerationNumber.fetch_add(1, std::memory_order_release);
	}
}

} // End of namespace lantern
