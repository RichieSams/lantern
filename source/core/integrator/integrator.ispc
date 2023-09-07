/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "integrator/integrator.h"

#include "camera/frame_data.h"
#include "camera/pinhole_camera.h"
#include "camera/pinhole_camera.isph"

#include "math/ray.isph"
#include "math/sampling.isph"

#include "integrator/surface_interaction.h"

#include "scene/scene.h"
#include "scene/scene.isph"

varying uint WangHash(varying uint a) {
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

uniform uint WangHashUniform(uniform uint a) {
	a = (a ^ 61) ^ (a >> 16);
	a = a + (a << 3);
	a = a ^ (a >> 4);
	a = a * 0x27d4eb2d;
	a = a ^ (a >> 15);
	return a;
}

varying float3 RayColor(uniform Scene *uniform scene, varying RNGState *uniform rngState, varying Ray &ray, int depth) {
	if (depth <= 0) {
		float3 ret = {0.0f, 0.0f, 0.0f};
		return ret;
	}

	SurfaceInteraction interaction;
	if (IntersectScene(scene, ray, 0.001, kInfinity, &interaction)) {
		float3 direction = UniformSampleHemisphere(normalize(interaction.Normal), rngState);
		Ray newRay = {interaction.Position, direction};

		return 0.5 * RayColor(scene, rngState, newRay, depth - 1);
	}

	// Calculate the background color
	float t = 0.5f * (ray.Direction.y + 1.0f);
	const float3 skyWhite = {1.0f, 1.0f, 1.0f};
	const float3 skyColor = {0.5f, 0.7f, 1.0f};
	return (1.0f - t) * skyWhite + t * skyColor;
}

export void RenderOneFrame(uniform Scene *uniform scene, uniform FrameData *uniform frameData, uniform uint frameNumber) {
	uniform int width = frameData->Width;
	uniform int height = frameData->Height;

	uniform uint frameNumHash = WangHashUniform(frameNumber);

	// "Render" a frame
	foreach (y = 0 ... height, x = 0 ... width) {
		RNGState rngState;

		uint pixelNumber = y * width + x;
		uint seed = WangHash(WangHash(pixelNumber) + frameNumHash);
		seed_rng(&rngState, seed);

		Ray ray = GenerateCameraRay(scene->Camera, &rngState, x, y);

		int maxDepth = 10;
		float3 color = RayColor(scene, &rngState, ray, maxDepth);

		int offset = y * width;
		frameData->ColorDataR[offset + x] += color.x;
		frameData->ColorDataG[offset + x] += color.y;
		frameData->ColorDataB[offset + x] += color.z;
		frameData->SampleCount[offset + x] += 1;
	}
}
