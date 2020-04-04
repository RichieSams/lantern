/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "math/vector_types.h"

#include "scene/scene.h"

#include "integrator/surface_interaction.h"

#include "materials/bsdf.h"
#include "materials/media/medium.h"

#include "embree3/rtcore.h"


namespace Lantern {

class Primitive {
public:
	virtual ~Primitive() = default;

public:
	float3 m_emission = float3(0.0f);
	BSDF *m_bsdf = nullptr;
	Medium *m_medium = nullptr;
	bool m_hasNormals = false;
	bool m_hasTexCoords = false;
	float m_surfaceArea = 0.0f;
	uint m_geometryId = 0;

public:
	void Initialize(float3 emission, BSDF *bsdf, Medium *medium, float surfaceArea, uint geometryId, bool hasNormals, bool hasTexCoords) {
		m_emission = emission;
		m_bsdf = bsdf;
		m_medium = medium;
		m_surfaceArea = surfaceArea;
		m_geometryId = geometryId;
		m_hasNormals = hasNormals;
		m_hasTexCoords = hasTexCoords;
	}

	virtual bool IsEmissive() const {
		return m_emission.x > 0.0f && m_emission.y > 0.0f && m_emission.z > 0.0f;
	}


	virtual bool HasNormals() const { return m_hasNormals; }
	virtual float3 InterpolateNormal(uint primId, float u, float v) = 0;

	virtual bool HasTexCoords() const { return m_hasTexCoords; }
	virtual float2 InterpolateTexCoords(uint primId, float u, float v) = 0;

	virtual float3 SampleDirectLighting(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *direction, float *distance, float *pdf) const = 0;
	virtual float PdfDirectLighting(Scene *scene, SurfaceInteraction &interaction, float3 inputDirection) const {
		// Check if the ray hits the light at all
		RTC_ALIGN(16) RTCRayHit rayHit;
		rayHit.ray.org_x = interaction.Position.x;
		rayHit.ray.org_y = interaction.Position.y;
		rayHit.ray.org_z = interaction.Position.z;

		// Reset the other ray properties
		rayHit.ray.dir_x = -inputDirection.x;
		rayHit.ray.dir_y = -inputDirection.y;
		rayHit.ray.dir_z = -inputDirection.z;

		rayHit.ray.tnear = 0.001f;
		rayHit.ray.tfar = embree::inf;
		rayHit.ray.mask = 0xFFFFFFFF;
		rayHit.ray.time = 0.0f;

		rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
		rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
		rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

		scene->Intersect(&rayHit);

		if (rayHit.hit.geomID == RTC_INVALID_GEOMETRY_ID) {
			return 0.0f;
		}

		float3 intersectionPoint = interaction.Position - inputDirection * rayHit.ray.tfar;
		float distanceSquared = sqr_length(intersectionPoint - interaction.Position);

		// Convert light sample weight to solid angle measure
		float pdf = distanceSquared / (std::abs(dot(normalize(float3(rayHit.hit.Ng_x, rayHit.hit.Ng_y, rayHit.hit.Ng_z)), -inputDirection)) * m_surfaceArea);

		if (std::isinf(pdf)) {
			return 0.0f;
		}

		return pdf;
	}
};

} // End of namespace Lantern
