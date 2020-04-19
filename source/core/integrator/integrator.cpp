/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "integrator/integrator.h"

#include "integrator/surface_interaction.h"

#include "scene/scene.h"

#include "primitives/primitive.h"
#include "materials/bsdf.h"
#include "materials/media/medium.h"

#include "math/uniform_sampler.h"
#include "math/vector_math.h"
#include "math/sampling.h"

#include "tbb/parallel_for.h"

#include <algorithm>


namespace Lantern {

void Integrator::RenderFrame() {
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

void Integrator::RenderTile(uint index, uint width, uint height, uint numTilesX, uint numTilesY) const {
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

enum class InteractionType {
	None,
	Surface,
	Medium
};

void Integrator::RenderPixel(uint x, uint y, UniformSampler *sampler) const {
	RTC_ALIGN(16) RTCRayHit rayHit;
	rayHit.ray = m_scene->Camera->CalculateRayFromPixel(x, y, sampler);
	
	rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

	float3 color(0.0f);
	float3 throughput(1.0f);

	SurfaceInteraction interaction;
	float currentIOR = 1.0f; // Air
	Medium *medium = nullptr;
	
	bool specularBounce = false;

	
	// Bounce the ray around the scene
	uint bounces = 0;
	const uint maxBounces = 1500;
	
	for (; bounces < maxBounces; ++bounces) {
		m_scene->Intersect(&rayHit);
		float3 origin = float3(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z);
		float3 direction = normalize(float3(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z));

		InteractionType interactionType = InteractionType::None;

//		// Check if there is a transmission event
//		if (medium != nullptr) {
//			float weight = 1.0f;
//			float pdf = 1.0f;
//			float distance = medium->SampleDistance(sampler, rayHit.ray.tfar, &weight, &pdf);
//			float3 transmission = medium->Transmission(distance);
//			throughput = throughput * weight * transmission;
//
//			if (distance < rayHit.ray.tfar) {
//				// Create a scatter event
//				interactionType = InteractionType::Medium;
//
//				float3a newOrigin = origin + direction * distance;
//				rayHit.ray.org_x = newOrigin.x;
//				rayHit.ray.org_y = newOrigin.y;
//				rayHit.ray.org_z = newOrigin.z;
//
//				// Reset the other ray properties
//				float directionPdf;
//				float3a newDirection = medium->SampleScatterDirection(sampler, direction, &directionPdf);
//				rayHit.ray.dir_x = newDirection.x;
//				rayHit.ray.dir_y = newDirection.y;
//				rayHit.ray.dir_z = newDirection.z;
//
//				rayHit.ray.tnear = 0.001f;
//				rayHit.ray.tfar = embree::inf;
//				rayHit.ray.mask = 0xFFFFFFFF;
//				rayHit.ray.time = 0.0f;
//
//				rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
//				rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
//				rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;
//			}
//		}

		// If we didn't scatter in the medium, check if we hit anything
		if (interactionType == InteractionType::None) {
			// Terminate the path if the ray escaped
			if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
				// Before terminating the path, check if this is the first bounce or if we just had a specular bounce
				// And add the environment lighting
				if (bounces == 0 || specularBounce) {
					color += throughput * m_scene->BackgroundColor;
				}
				break;
			}
			
			interactionType = InteractionType::Surface;
			Primitive *primitive = m_scene->GetPrimitive(rayHit.hit.geomID);

			// If this is the first bounce or if we just had a specular bounce,
			// we need to add the emmisive light
			if (bounces == 0 || specularBounce) {
				// Add emitted light at path vertex or from the environment
				color += throughput * primitive->m_emission;
			}

			// We hit an object. Calculate the surface interaction
			interaction.Position = origin + direction * rayHit.ray.tfar;
			interaction.Normal = normalize(float3(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z));
			interaction.OutputDirection = -direction;

			if (primitive->HasNormals()) {
				interaction.Shading.Normal = normalize(primitive->InterpolateNormal(rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v));
			}
			else {
				interaction.Shading.Normal = interaction.Normal;
			}
			const bool backface = dot(interaction.Normal, interaction.OutputDirection) < 0;
			if (backface) {
				interaction.Normal *= -1.0f;
				interaction.Shading.Normal *= -1.0f;
			}
			
			if (primitive->HasTexCoords()) {
				interaction.TexCoord = primitive->InterpolateTexCoords(rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v);
			} else {
				interaction.TexCoord = float2(0.0f, 0.0f);
			}

			BSDF *bsdf = primitive->m_bsdf;

			// Calculate the direct lighting
			color += throughput * SampleOneLight(sampler, m_scene, interaction, bsdf, currentIOR, primitive);

			// Get the new ray direction
			// Choose the direction based on the bsdf
			float3 inputDirection;
			float pdf;
			BSDFLobe::Type sampledLobe;
			float IORo;
			float3 f = bsdf->Sample(sampler, interaction, &inputDirection, &pdf, BSDFLobe::All, &sampledLobe, currentIOR, &IORo );

			// Accumulate the weight
			throughput = throughput * f * abs(dot(inputDirection, interaction.Shading.Normal)) / pdf;

			// Update the current IOR and medium if we refracted
			if ((sampledLobe & BSDFLobe::Transmission) == BSDFLobe::Transmission) {
				currentIOR = IORo;
				medium = primitive->m_medium;
			}

			// Check for specular reflection
			specularBounce = (sampledLobe & BSDFLobe::Specular) == BSDFLobe::Specular;

			// Create the new ray

			// Set the origin at the intersection point
			rayHit.ray.org_x = interaction.Position.x;
			rayHit.ray.org_y = interaction.Position.y;
			rayHit.ray.org_z = interaction.Position.z;

			// Reset the other ray properties
			rayHit.ray.dir_x = inputDirection.x;
			rayHit.ray.dir_y = inputDirection.y;
			rayHit.ray.dir_z = inputDirection.z;

			rayHit.ray.tnear = 0.001f;
			rayHit.ray.tfar = std::numeric_limits<float>::infinity();
			rayHit.ray.mask = 0xFFFFFFFF;
			rayHit.ray.time = 0.0f;

			rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
			rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
			rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;
		}

		// Russian Roulette
		if (bounces > 3) {
			float p = std::max((float)throughput.x, std::max((float)throughput.y, (float)throughput.z));
			if (sampler->NextFloat() > p) {
				break;
			}

			throughput *= 1 / p;
		}
	}

	size_t index = y * m_currentFrameBuffer->Width + x;

	m_currentFrameBuffer->ColorData[index] += color;
	m_currentFrameBuffer->Bounces[index] += bounces;
	m_currentFrameBuffer->ColorSampleCount[index] += 1u;
}

float3 Integrator::SampleOneLight(UniformSampler *sampler, Scene *scene, SurfaceInteraction interaction, BSDF *bsdf, float IORi, Primitive *hitPrimitive) const {
	std::size_t numLights = m_scene->NumLights();
	
	// Return black if there are no lights
	// And don't let a light contribute light to itself
	// Aka, if we hit a light
	// This is the special case where there is only 1 light
	if (numLights == 0 || numLights == 1 && hitPrimitive->IsEmissive()) {
		return float3(0.0f);
	}

	// Don't let a light contribute light to itself
	// Choose another one
	Primitive *light;
	do {
		light = m_scene->RandomOneLight(sampler);
	} while (light == hitPrimitive);

	const float lightPdf = 1.0f / numLights;
	return EstimateDirect(light, sampler, scene, interaction, bsdf, IORi) / lightPdf;
}

float3 Integrator::EstimateDirect(Primitive *light, UniformSampler *sampler, Scene *scene, SurfaceInteraction &interaction, BSDF *bsdf, float IORi) const {
	BSDFLobe::Type allowedLobes = BSDFLobe::Type(BSDFLobe::All & ~BSDFLobe::Specular);
	
	float3 directLighting = float3(0.0f);
	float3 inputDirection;
	float lightPdf, scatteringPdf;

	// Sample lighting with multiple importance sampling
	float distance;
	float3 Li = light->SampleDirectLighting(sampler, interaction, &inputDirection, &distance, &lightPdf);
	if (lightPdf > 0.0f && !all(Li)) {
		// Evaluate BSDF for sampled input direction
		float3 f = bsdf->Eval(interaction, inputDirection, allowedLobes);
		scatteringPdf = bsdf->Pdf(interaction, inputDirection, allowedLobes);

		if (!all(f)) {
			// Check visibility from the intersection to the sampled location on the light
			RTC_ALIGN(16) RTCRay ray;
			memset(&ray, 0, sizeof(ray));

			ray.org_x = interaction.Position.x;
			ray.org_y = interaction.Position.y;
			ray.org_z = interaction.Position.z;

			ray.tnear = 0.001f;
			ray.tfar = distance - 0.001f;
			ray.mask = 0xFFFFFFFF;
			scene->Occluded(&ray);

			if (ray.tfar != distance - 0.001f) {
				Li = float3(0.0f);
			}

			// Add the light's contribution to the reflected radiance
			if (!all(Li)) {
				float weight = PowerHeuristic(1, lightPdf, 1, scatteringPdf);
				directLighting += f * Li * weight / lightPdf;
			}
		}
	}

	// Sample BSDF with multiple importance sampling
	BSDFLobe::Type sampledLobe;
	float IORo;
	float3 f = bsdf->Sample(sampler, interaction, &inputDirection, &scatteringPdf, allowedLobes, &sampledLobe, IORi, &IORo);
	f *= std::abs(dot(inputDirection, interaction.Shading.Normal));
	bool sampledSpecular = sampledLobe & BSDFLobe::Specular;

	if (!all(f) && scatteringPdf > 0.0f) {
		// Account for light contributions along the sampled inputDirection
		float weight = 1.0f;
		if (!sampledSpecular) {
			lightPdf = light->PdfDirectLighting(scene, interaction, inputDirection);
			if (lightPdf == 0.0f) {
				return directLighting;
			}

			weight = PowerHeuristic(1, scatteringPdf, 1, lightPdf);
		}

		// Check if the ray hits the light at all
		RTC_ALIGN(16) RTCRayHit rayHit;
		rayHit.ray.org_x = interaction.Position.x;
		rayHit.ray.org_y = interaction.Position.y;
		rayHit.ray.org_z = interaction.Position.z;

		rayHit.ray.dir_x = -inputDirection.x;
		rayHit.ray.dir_y = -inputDirection.y;
		rayHit.ray.dir_z = -inputDirection.z;

		rayHit.ray.tnear = 0.001f;
		rayHit.ray.tfar = std::numeric_limits<float>::infinity();
		rayHit.ray.mask = 0xFFFFFFFF;
		rayHit.ray.time = 0.0f;

		rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
		rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
		rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

		scene->Intersect(&rayHit);

		Li = float3(0.0f);
		if (rayHit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
			if (scene->GetPrimitive(rayHit.hit.geomID) == light) {
				Li = light->m_emission;
			}
		}

		if (!all(Li)) {
			directLighting += f * Li * weight / scatteringPdf;
		}
	}

	return directLighting;
}


} // End of namespace Lantern
