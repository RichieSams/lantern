/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "integrator/integrator.h"

#include "camera/frame_data.h"
#include "camera/pinhole_camera.h"
#include "camera/pinhole_camera.isph"

#include "math/ray.h"

#include "integrator/surface_interaction.h"

#include "scene/scene.h"
#include "scene/scene.isph"

export void RenderOneFrame(uniform Scene *uniform scene, uniform FrameData *uniform frameData) {
	uniform int width = frameData->Width;
	uniform int height = frameData->Height;

	// "Render" a frame
	foreach (y = 0 ... height, x = 0 ... width) {
		Ray ray = GenerateCameraRay(scene->Camera, x, y);

		// Calculate the background color
		float t = 0.5f * (ray.Direction.y + 1.0f);
		const float3 skyWhite = {1.0f, 1.0f, 1.0f};
		const float3 skyColor = {0.5f, 0.7f, 1.0f};
		float3 color = (1.0f - t) * skyWhite + t * skyColor;

		SurfaceInteraction interaction;
		if (IntersectScene(scene, ray, 0, kInfinity, &interaction)) {
			color = 0.5f * (normalize(interaction.Normal) + 1.0f);
		}

		int offset = y * width;
		frameData->ColorDataR[offset + x] += color.x;
		frameData->ColorDataG[offset + x] += color.y;
		frameData->ColorDataB[offset + x] += color.z;
		frameData->SampleCount[offset + x] += 1;
	}
}
