/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "integrator/integrator.h"

namespace lantern {

// 16 color palette
float3 palette[] = {
    {1.000f, 1.000f, 1.000f}, // White
    {0.984f, 0.953f, 0.020f}, // Yellow - #fbf305
    {1.000f, 0.392f, 0.012f}, // Orange - #ff6403
    {0.867f, 0.035f, 0.027f}, // Red - #dd0907
    {0.949f, 0.031f, 0.518f}, // Magenta - #f20884
    {0.278f, 0.000f, 0.647f}, // Purple - #4700a5
    {0.000f, 0.000f, 0.827f}, // Blue - #0000d3
    {0.008f, 0.671f, 0.918f}, // Cyan - #02abea
    {0.122f, 0.718f, 0.078f}, // Green - #1fb714
    {0.000f, 0.392f, 0.071f}, // Dark Green - #006412
    {0.337f, 0.173f, 0.020f}, // Brown - #562c05
    {0.565f, 0.443f, 0.227f}, // Tan - #90713a
    {0.753f, 0.753f, 0.753f}, // Light Gray - #C0C0C0
    {0.502f, 0.502f, 0.502f}, // Medium Gray - #808080
    {0.251f, 0.251f, 0.251f}, // Dark Gray - #404040
    {0.000f, 0.000f, 0.000f}, // Black
};

Integrator::Integrator(FrameData *startingFrameData, std::atomic<FrameData *> *swapFrameData)
        : m_currentFrameData(startingFrameData),
          m_swapFrameData(swapFrameData) {
}

void Integrator::RenderOneFrame() {
	m_currentFrameData = std::atomic_exchange(m_swapFrameData, m_currentFrameData);
	m_currentFrameData->Empty = false;

	const uint32_t width = m_currentFrameData->Width;
	const uint32_t height = m_currentFrameData->Height;

	// "Render" a frame
	for (uint32_t y = 0; y < height; ++y) {
		const uint32_t offset = y * width;
		for (uint32_t x = 0; x < width; ++x) {
			uint32_t xColorIndex = x / 32;
			uint32_t yColorIndex = y / 32;

			const float3 color = palette[(xColorIndex + yColorIndex) % 16];
			m_currentFrameData->ColorData[offset + x] += color;
			m_currentFrameData->SampleCount[offset + x] += 1;
		}
	}
}

} // namespace lantern
