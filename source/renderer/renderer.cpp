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
#include "math/vector_math.h"
#include "math/sampling.h"

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
	RTCRay ray = m_scene->Camera.CalculateRayFromPixel(x, y, sampler);

	float3 color(0.0f);
	float3 throughput(1.0f);

	// Bounce the ray around the scene
	for (uint bounces = 0; bounces < 10; ++bounces) {
		m_scene->Intersect(ray);

		// The ray missed. Return the background color
		if (ray.geomID == RTC_INVALID_GEOMETRY_ID) {
			//color += throughput * float3(0.846f, 0.933f, 0.949f);
			break;
		}

		// We hit an object
			
		// Fetch the material
		Material *material = m_scene->GetMaterial(ray.geomID);
		// The object might be emissive. If so, it will have a corresponding light
		// Otherwise, GetLight will return nullptr
		Light *light = m_scene->GetLight(ray.geomID);

		// If this is the first bounce, we need to add the emmisive light
		if (bounces == 0 && light != nullptr) {
			color += throughput * light->Le();
		}

		float3a normal = normalize(ray.Ng);
		float3a wo = normalize(-ray.dir);
		float3a surfacePos = ray.org + ray.dir * ray.tfar;

		// Calculate the direct lighting
		color += throughput * SampleOneLight(sampler, surfacePos, normal, wo, material, light);


		// Get the new ray direction
		// Choose the direction based on the material
		float3a wi = material->Sample(wo, normal, sampler);
		float pdf = material->Pdf(wi, normal);

		// Accumulate the diffuse/specular weight
		throughput = throughput * material->Eval(wi, wo, normal) / pdf;

		// Russian Roulette
		if (bounces > 3) {
			float p = std::max(throughput.x, std::max(throughput.y, throughput.z));
			if (sampler->NextFloat() > p) {
				break;
			}

			throughput *= 1 / p;
		}

		// Shoot a new ray

		// Set the origin at the intersection point
		ray.org = surfacePos;

		// Reset the other ray properties
		ray.dir = wi;
		ray.tnear = 0.001f;
		ray.tfar = embree::inf;
		ray.geomID = RTC_INVALID_GEOMETRY_ID;
		ray.primID = RTC_INVALID_GEOMETRY_ID;
		ray.instID = RTC_INVALID_GEOMETRY_ID;
		ray.mask = 0xFFFFFFFF;
		ray.time = 0.0f;
	}

	m_scene->Camera.FrameBuffer.SplatPixel(x, y, color);
}

float3 Renderer::SampleOneLight(UniformSampler *sampler, float3a &surfacePos, float3a &surfaceNormal, float3a &wo, Material *material, Light *hitLight) const {
	std::size_t numLights = m_scene->NumLights();
	
	// Return black if there are no lights
	// And don't let a light contribute light to itself
	// Aka, if we hit a light
	// This is the special case where there is only 1 light
	if (numLights == 0 || numLights == 1 && hitLight != nullptr) {
		return float3(0.0f);
	}

	// Don't let a light contribute light to itself
	// Choose another one
	Light *light;
	do {
		light = m_scene->RandomOneLight(sampler);
	} while (light == hitLight);

	return numLights * EstimateDirect(light, sampler, surfacePos, surfaceNormal, wo, material);
}

float3 Renderer::EstimateDirect(Light *light, UniformSampler *sampler, float3a &surfacePos, float3a &surfaceNormal, float3a &wo, Material *material) const {
	float3 directLighting = float3(0.0f);
	float3a wi;
	float3 f;
	float lightPdf, scatteringPdf;


	// Sample lighting with multiple importance sampling

	float3 Li = light->SampleLi(sampler, m_scene, surfacePos, surfaceNormal, &wi, &lightPdf);

	// Make sure the pdf isn't zero and the radiance isn't black
	if (lightPdf != 0.0f && !all(Li)) {
		// Calculate the brdf value
		f = material->Eval(wi, wo, surfaceNormal);
		scatteringPdf = material->Pdf(wi, surfaceNormal);

		if (scatteringPdf != 0.0f && !all(f)) {
			float weight = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
			directLighting += f * Li * weight / lightPdf;
		}
	}


	// Sample brdf with multiple importance sampling

	wi = material->Sample(wo, surfaceNormal, sampler);
	f = material->Eval(wi, wo, surfaceNormal);
	scatteringPdf = material->Pdf(wo, surfaceNormal);
	if (scatteringPdf != 0.0f && !all(f)) {
		lightPdf = light->PdfLi(m_scene, surfacePos, surfaceNormal, wi);
		if (lightPdf == 0.0f) {
			// We didn't hit anything, so ignore the brdf sample
			return directLighting;
		}

		float weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
		Li = light->Le();
		directLighting += f * Li * weight / scatteringPdf;
	}

	return directLighting;
}


} // End of namespace Lantern
