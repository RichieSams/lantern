/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "renderer/renderer.h"

#include "renderer/surface_interaction.h"

#include "scene/scene.h"

#include "materials/material.h"
#include "materials/bsdfs/bsdf.h"
#include "materials/media/medium.h"

#include "math/uniform_sampler.h"
#include "math/vector_math.h"
#include "math/sampling.h"

#include "tbb/parallel_for.h"

#include <algorithm>


namespace Lantern {

void Renderer::RenderFrame() {
	uint width = m_scene->Camera->FrameBufferWidth;
	uint height = m_scene->Camera->FrameBufferHeight;

	const uint numTilesX = (width + kTileSize - 1) / kTileSize;
	const uint numTilesY = (height + kTileSize - 1) / kTileSize;

	m_currentFrameBuffer = std::atomic_exchange(m_swapFrameBuffer, m_currentFrameBuffer);
	m_currentFrameBuffer->Empty = false;

	tbb::parallel_for(uint(0), uint(numTilesX * numTilesY), [=](uint i) {
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

void Renderer::RenderTile(uint index, uint width, uint height, uint numTilesX, uint numTilesY) const {
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

void Renderer::RenderPixel(uint x, uint y, UniformSampler *sampler) const {
	RTC_ALIGN(16) RTCRayHit rayHit;
	rayHit.ray = m_scene->Camera->CalculateRayFromPixel(x, y, sampler);
	rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

	float3 color(0.0f);
	float3 throughput(1.0f);
	SurfaceInteraction interaction;
	interaction.IORi = 1.0f; // Air
	Medium *medium = nullptr;
	bool hitSurface = false;

	// Bounce the ray around the scene
	uint bounces = 0;
	const uint maxBounces = 1500;
	for (; bounces < maxBounces; ++bounces) {
		m_scene->Intersect(rayHit);

		// The ray missed. Return the background color
		if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
			color += throughput * m_scene->BackgroundColor;
			break;
		}

		float3a origin = float3a(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
		float3a direction = normalize(float3a(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z));

		// We hit an object
		hitSurface = true;
		
		// Calculate any transmission
		if (medium != nullptr) {
			float weight = 1.0f;
			float pdf = 1.0f;
			float distance = medium->SampleDistance(sampler, rayHit.ray.tfar, &weight, &pdf);
			float3 transmission = medium->Transmission(distance);
			throughput = throughput * weight * transmission;

			if (distance < rayHit.ray.tfar) {
				// Create a scatter event
				hitSurface = false;

				float3a newOrigin = origin + direction * distance;
				rayHit.ray.org_x = newOrigin.x;
				rayHit.ray.org_y = newOrigin.y;
				rayHit.ray.org_z = newOrigin.z;

				// Reset the other ray properties
				float directionPdf;
				float3a newDirection = medium->SampleScatterDirection(sampler, direction, &directionPdf);
				rayHit.ray.dir_x = newDirection.x;
				rayHit.ray.dir_y = newDirection.y;
				rayHit.ray.dir_z = newDirection.z;

				rayHit.ray.tnear = 0.001f;
				rayHit.ray.tfar = embree::inf;
				rayHit.ray.mask = 0xFFFFFFFF;
				rayHit.ray.time = 0.0f;

				rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
				rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
				rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;
			}
		}

		if (hitSurface) {
			// Fetch the material
			Material *material = m_scene->GetMaterial(rayHit.hit.geomID);
			// The object might be emissive. If so, it will have a corresponding light
			// Otherwise, GetLight will return nullptr
			Light *light = m_scene->GetLight(rayHit.hit.geomID);

			// If this is the first bounce or if we just had a specular bounce,
			// we need to add the emmisive light
			if ((bounces == 0 || (interaction.SampledLobe & BSDFLobe::Specular) != 0) && light != nullptr) {
				color += throughput * light->Le();
			}

			interaction.Position = origin + direction * rayHit.ray.tfar;
			if (m_scene->HasNormals(rayHit.hit.geomID)) {
				interaction.Normal = normalize(m_scene->InterpolateNormal(rayHit.hit.geomID, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v));
			} else {
				interaction.Normal = normalize(float3a());
			}
			if (m_scene->HasTexCoords(rayHit.hit.geomID)) {
				interaction.TexCoord = m_scene->InterpolateTexCoord(rayHit.hit.geomID, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v);
			} else {
				interaction.TexCoord = float2(0.0f, 0.0f);
			}
			interaction.OutputDirection = -direction;
			interaction.IORo = 0.0f;


			// Calculate the direct lighting
			color += throughput * SampleOneLight(sampler, interaction, material->bsdf, light);


			// Get the new ray direction
			// Choose the direction based on the bsdf		
			material->bsdf->Sample(interaction, sampler);
			float pdf = material->bsdf->Pdf(interaction);

			// Accumulate the weight
			throughput = throughput * material->bsdf->Eval(interaction) / pdf;

			// Update the current IOR and medium if we refracted
			if (interaction.SampledLobe == BSDFLobe::SpecularTransmission) {
				interaction.IORi = interaction.IORo;
				medium = material->medium;
			}

			// Shoot a new ray

			// Set the origin at the intersection point
			rayHit.ray.org_x = interaction.Position.x;
			rayHit.ray.org_y = interaction.Position.y;
			rayHit.ray.org_z = interaction.Position.z;

			// Reset the other ray properties
			float directionPdf;
			rayHit.ray.dir_x = interaction.InputDirection.x;
			rayHit.ray.dir_y = interaction.InputDirection.y;
			rayHit.ray.dir_z = interaction.InputDirection.z;

			rayHit.ray.tnear = 0.001f;
			rayHit.ray.tfar = embree::inf;
			rayHit.ray.mask = 0xFFFFFFFF;
			rayHit.ray.time = 0.0f;

			rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
			rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
			rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;
		}

		// Russian Roulette
		if (bounces > 3) {
			float p = std::max(throughput.x, std::max(throughput.y, throughput.z));
			if (sampler->NextFloat() > p) {
				break;
			}

			throughput *= 1 / p;
		}
	}

	size_t index = y * m_currentFrameBuffer->Width + x;

	m_currentFrameBuffer->ColorData[index] += color;
	m_currentFrameBuffer->Bounces[index] += bounces;
	m_currentFrameBuffer->Weights[index] += 1.0f;
}

float3 Renderer::SampleOneLight(UniformSampler *sampler, SurfaceInteraction interaction, BSDF *bsdf, Light *hitLight) const {
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

	return (float)numLights * EstimateDirect(light, sampler, interaction, bsdf);
}

float3 Renderer::EstimateDirect(Light *light, UniformSampler *sampler, SurfaceInteraction &interaction, BSDF *bsdf) const {
	float3 directLighting = float3(0.0f);
	float3 f;
	float lightPdf, scatteringPdf;


	// Sample lighting with multiple importance sampling
	// Only sample if the BRDF is non-specular 
	if ((bsdf->SupportedLobes & ~BSDFLobe::Specular) != 0) {
		float3 Li = light->SampleLi(sampler, m_scene, interaction, &lightPdf);

		// Make sure the pdf isn't zero and the radiance isn't black
		if (lightPdf != 0.0f && !all(Li)) {
			// Calculate the brdf value
			f = bsdf->Eval(interaction);
			scatteringPdf = bsdf->Pdf(interaction);

			if (scatteringPdf != 0.0f && !all(f)) {
				float weight = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
				directLighting += f * Li * weight / lightPdf;
			}
		}
	}


	// Sample brdf with multiple importance sampling
	bsdf->Sample(interaction, sampler);
	f = bsdf->Eval(interaction);
	scatteringPdf = bsdf->Pdf(interaction);
	if (scatteringPdf != 0.0f && !all(f)) {
		lightPdf = light->PdfLi(m_scene, interaction);
		if (lightPdf == 0.0f) {
			// We didn't hit anything, so ignore the brdf sample
			return directLighting;
		}

		float weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
		float3 Li = light->Le();
		directLighting += f * Li * weight / scatteringPdf;
	}

	return directLighting;
}


} // End of namespace Lantern
