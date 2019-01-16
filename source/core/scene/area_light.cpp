/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/area_light.h"

#include "scene/scene.h"

#include "math/sampling.h"

#include "integrator/surface_interaction.h"


namespace Lantern {

float3 AreaLight::SampleLi(UniformSampler *sampler, Scene *scene, SurfaceInteraction &interaction, float *pdf) const {
	// Generate a random point on the great circle of the bounding sphere that is oriented towards the origin
	float x, y;
	UniformSampleDisc(sampler, m_boundingSphere.w, &x, &y);
	
	// Calculate the paramaters of the great circle
	float3a circleDirection = (float3a)m_boundingSphere.xyz() - interaction.Position;
	float distance = length(circleDirection);
	float3a circleNormal = normalize(circleDirection);

	float3a point = RotateToWorld(x, y, distance - m_boundingSphere.w, circleNormal);
	float3a direction = normalize(point);
	interaction.InputDirection = direction;

	// Check that the point on the circle is above the horizon
	if (dot(direction, interaction.Normal) <= 0.0f) {
		*pdf = 0.0f;
		return float3(0.0f);
	}

	// Make sure there's nothing occluding us
	RTC_ALIGN(16) RTCRayHit rayHit;
	memset(&rayHit, 0, sizeof(rayHit));

	rayHit.ray.org_x = interaction.Position.x;
	rayHit.ray.org_y = interaction.Position.y;
	rayHit.ray.org_z = interaction.Position.z;

	rayHit.ray.dir_x = direction.x;
	rayHit.ray.dir_y = direction.y;
	rayHit.ray.dir_z = direction.z;

	rayHit.ray.tnear = 0.001f;
	rayHit.ray.tfar = embree::inf;
	rayHit.ray.mask = 0xFFFFFFFF;

	rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

	scene->Intersect(rayHit);
	if (rayHit.hit.geomID != m_geomId) {
		*pdf = 0.0f;
		return float3(0.0f);
	}

	// Calculate the pdf
	float3a intersectionPoint = float3a(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z) + normalize(float3a(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z)) * rayHit.ray.tfar;
	float distanceSquared = sqr_length(intersectionPoint - interaction.Position);
	*pdf = distanceSquared / (std::abs(dot(normalize(scene->InterpolateNormal(rayHit.hit.geomID, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v)), -direction)) * m_area);

	// Return the full radiance value
	// The value will be attenuated by the BRDF
	return m_radiance;
}

float AreaLight::PdfLi(Scene *scene, SurfaceInteraction &interaction) const {
	// Check that wi is above the horizon
	if (dot(interaction.InputDirection, interaction.Normal) <= 0.0f) {
		return 0.0f;
	}

	// Make sure there's nothing occluding us
	RTC_ALIGN(16) RTCRayHit rayHit;
	memset(&rayHit, 0, sizeof(rayHit));

	rayHit.ray.org_x = interaction.Position.x;
	rayHit.ray.org_y = interaction.Position.y;
	rayHit.ray.org_z = interaction.Position.z;

	rayHit.ray.dir_x = interaction.InputDirection.x;
	rayHit.ray.dir_y = interaction.InputDirection.y;
	rayHit.ray.dir_z = interaction.InputDirection.z;

	rayHit.ray.tnear = 0.001f;
	rayHit.ray.tfar = embree::inf;
	rayHit.ray.mask = 0xFFFFFFFF;

	rayHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.instID[0] = RTC_INVALID_GEOMETRY_ID;
	rayHit.hit.primID = RTC_INVALID_GEOMETRY_ID;

	scene->Intersect(rayHit);
	if (rayHit.hit.geomID != m_geomId) {
		return 0.0f;
	}

	// Calculate the pdf
	float3a intersectionPoint = float3a(rayHit.ray.org_x, rayHit.ray.org_y, rayHit.ray.org_z) + normalize(float3a(rayHit.ray.dir_x, rayHit.ray.dir_y, rayHit.ray.dir_z)) * rayHit.ray.tfar;
	float distanceSquared = sqr_length(intersectionPoint - interaction.Position);
	
	return distanceSquared / (std::abs(dot(normalize(scene->InterpolateNormal(rayHit.hit.geomID, rayHit.hit.primID, rayHit.hit.u, rayHit.hit.v)), interaction.InputDirection)) * m_area);
}

} // End of namespace Lantern
