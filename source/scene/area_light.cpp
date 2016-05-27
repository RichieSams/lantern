/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/area_light.h"

#include "scene/scene.h"

#include "math/sampling.h"

#include "renderer/surface_interaction.h"


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
	Ray ray;
	ray.Origin = interaction.Position;
	ray.Direction = direction;
	ray.TNear = 0.001f;
	ray.TFar = embree::inf;
	ray.GeomID = INVALID_GEOMETRY_ID;
	ray.PrimID = INVALID_PRIMATIVE_ID;
	ray.InstID = INVALID_INSTANCE_ID;
	ray.Mask = 0xFFFFFFFF;
	ray.Time = 0.0f;

	scene->Intersect(ray);
	if (ray.GeomID != m_geomId) {
		*pdf = 0.0f;
		return float3(0.0f);
	}

	// Calculate the pdf
	float3a intersectionPoint = ray.Origin + ray.Direction * ray.TFar;
	float distanceSquared = sqr_length(intersectionPoint - interaction.Position);
	*pdf = distanceSquared / (std::abs(dot(normalize(ray.GeomNormal), -direction)) * m_area);

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
	Ray ray;
	ray.Origin = interaction.Position;
	ray.Direction = interaction.InputDirection;
	ray.TNear = 0.001f;
	ray.TFar = infinity;
	ray.GeomID = INVALID_GEOMETRY_ID;
	ray.PrimID = INVALID_PRIMATIVE_ID;
	ray.InstID = INVALID_INSTANCE_ID;
	ray.Mask = 0xFFFFFFFF;
	ray.Time = 0.0f;

	scene->Intersect(ray);
	if (ray.GeomID != m_geomId) {
		return 0.0f;
	}

	// Calculate the pdf
	float3a intersectionPoint = ray.Origin + ray.Direction * ray.TFar;
	float distanceSquared = sqr_length(intersectionPoint - interaction.Position);
	
	return distanceSquared / (std::abs(dot(normalize(ray.GeomNormal), interaction.InputDirection)) * m_area);
}

} // End of namespace Lantern
