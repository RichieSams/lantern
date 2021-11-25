/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "integrator/integrator.h"

#include "camera/frame_data.h"

#include "integrator/surface_interaction.h"

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

Integrator::Integrator(uint32_t width, uint32_t height, Scene *scene)
        : m_camera(width, height, 90.0f),
          m_scene(scene) {
}

void Integrator::RenderOneFrame(FrameData *dest) {
	const uint32_t width = dest->Width;
	const uint32_t height = dest->Height;

	// "Render" a frame
	for (uint32_t y = 0; y < height; ++y) {
		const uint32_t offset = y * width;
		for (uint32_t x = 0; x < width; ++x) {
			Ray ray = m_camera.GetRay(x, y);
			float3 color = (normalize(ray.Direction) + 1.0f) * 0.5f;

			SurfaceInteraction interaction;
			if (m_scene->Interesect(ray, 0, kInfinity, &interaction)) {
				color = (normalize(interaction.Normal) + 1.0f) * 0.5f;
			}

			dest->ColorData[offset + x] += color;
			dest->SampleCount[offset + x] += 1;
		}
	}
}

} // namespace lantern
