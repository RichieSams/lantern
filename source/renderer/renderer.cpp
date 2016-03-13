/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "renderer/renderer.h"

#include "scene/scene.h"
#include "scene/ray.h"

#include "materials/material.h"

#include "math/uniform_sampler.h"

#include <tbb/parallel_for.h>

#include <algorithm>


namespace Lantern {

void Renderer::RenderFrame() {
	uint width = m_scene->Camera.FrameBuffer.Width;
	uint height = m_scene->Camera.FrameBuffer.Height;

	const int numTilesX = (width + kTileSize - 1) / kTileSize;
	const int numTilesY = (height + kTileSize - 1) / kTileSize;

	tbb::parallel_for(size_t(0), size_t(numTilesX * numTilesY), [=](size_t i) {
		RenderTile(i, width, height, numTilesX, numTilesY);
	});

	++m_frameNumber;
}

// MurmurHash3
inline uint HashMix(uint hash, uint k) {
	const uint c1 = 0xcc9e2d51;
	const uint c2 = 0x1b873593;
	const uint r1 = 15;
	const uint r2 = 13;
	const uint m = 5;
	const uint n = 0xe6546b64;

	k *= c1;
	k = (k << r1) | (k >> (32 - r1));
	k *= c2;

	hash ^= k;
	hash = ((hash << r2) | (hash >> (32 - r2))) * m + n;

	return hash;
}

// MurmurHash3
inline uint HashFinalize(uint hash) {
	hash ^= hash >> 16;
	hash *= 0x85ebca6b;
	hash ^= hash >> 13;
	hash *= 0xc2b2ae35;
	hash ^= hash >> 16;

	return hash;
}

void Renderer::RenderTile(uint index, uint width, uint height, uint numTilesX, uint numTilesY) {
	uint tileY = index / numTilesX;
	uint tileX = index - tileY * numTilesX;

	uint x0 = tileX * kTileSize;
	uint x1 = std::min(x0 + kTileSize, width);
	uint y0 = tileY * kTileSize;
	uint y1 = std::min(y0 + kTileSize, height);

	uint hash = 0u;
	hash = HashMix(hash, index);
	hash = HashMix(hash, m_frameNumber);
	hash = HashFinalize(hash);
	
	UniformSampler sampler(hash, m_frameNumber);

	for (uint y = y0; y < y1; ++y) {
		for (uint x = x0; x < x1; ++x) {
			RenderPixel(x, y, &sampler);
		}
	}
}

void Renderer::RenderPixel(uint x, uint y, UniformSampler *sampler) {
	RTCRay ray = m_scene->Camera.CalculateRayFromPixel(x, y);

	float3 color(0.0f);
	float3 weights(1.0f);

	// Bounce the ray around the scene
	for (uint bounces = 0; bounces < 10; ++bounces) {
		m_scene->Intersect(ray);

		if (ray.geomID != RTC_INVALID_GEOMETRY_ID) {
			// We hit an object
			
			// Fetch the material
			Material *material = m_scene->GetMaterial(ray.geomID);

			// Add the emmisive light
			color += weights * material->EmissiveColor();

			// Get the new ray direction
			// Choose the direction based on the material
			float pdf;
			float3a normal = normalize(ray.Ng);
			float3a wi = material->Sample(ray.dir, normal, sampler, &pdf);

			// Accumulate the diffuse/specular weight
			weights = weights * material->Eval(wi, normalize(ray.dir), normal) / pdf;

			// Russian Roulette
			if (bounces > 3) {
				float p = std::max(weights.x, std::max(weights.y, weights.z));
				if (sampler->NextFloat() > p) {
					break;
				}

				weights *= 1 / p;
			}

			// Shoot a new ray

			// Set the origin at the intersection point
			ray.org = ray.org + ray.dir * ray.tfar;

			// Reset the other ray properties
			ray.dir = wi;
			ray.tnear = 0.001f;
			ray.tfar = embree::inf;
			ray.geomID = RTC_INVALID_GEOMETRY_ID;
			ray.primID = RTC_INVALID_GEOMETRY_ID;
			ray.instID = RTC_INVALID_GEOMETRY_ID;
			ray.mask = 0xFFFFFFFF;
			ray.time = 0.0f;

		} else {
			// We didn't hit anything, return the sky color
			color += weights * float3(0.846f, 0.933f, 0.949f);

			break;
		}
	}
	
	m_scene->Camera.FrameBuffer.SplatPixel(x, y, color);
}



} // End of namespace Lantern
