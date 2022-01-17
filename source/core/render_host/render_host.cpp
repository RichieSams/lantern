/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "render_host/render_host.h"

#include "render_host/presentation_buffer.h"

#include "scene/scene.h"

#include "integrator/integrator.h"

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
	while (!quit->load(std::memory_order_relaxed)) {
		RenderOneFrame(m_scene, &m_accumulationBuffer);

		// Now resolve the new data
		for (uint32_t y = 0; y < m_accumulationBuffer.Height; ++y) {
			const uint32_t offset = y * m_accumulationBuffer.Width;
			for (uint32_t x = 0; x < m_accumulationBuffer.Width; ++x) {
				const uint32_t frameBufferIndex = offset + x;
				const uint32_t resolvedDataIndex = frameBufferIndex * 3;

				float3 color = m_accumulationBuffer.ColorData[frameBufferIndex];
				const uint32_t sampleCount = m_accumulationBuffer.SampleCount[frameBufferIndex];

				// Color data
				m_currentPresentationBuffer->ResolvedData[resolvedDataIndex + 0] = color.x / sampleCount; // Red
				m_currentPresentationBuffer->ResolvedData[resolvedDataIndex + 1] = color.y / sampleCount; // Green
				m_currentPresentationBuffer->ResolvedData[resolvedDataIndex + 2] = color.z / sampleCount; // Blue

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