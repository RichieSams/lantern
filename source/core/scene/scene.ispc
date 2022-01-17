/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "scene/scene.h"

#include "math/constants.h"
#include "math/ray.h"

#include "integrator/surface_interaction.h"

#include "scene/sphere.isph"


varying bool IntersectScene(uniform Scene * uniform scene, varying Ray &ray, varying float minDistance, varying float maxDistance, varying SurfaceInteraction * varying surfaceInteraction) {
	varying float distance = kInfinity;
	varying float3 normal = {0.0f, 0.0f, 0.0f};

	for (uniform size_t i = 0; i < scene->HittablesLen; ++i) {
		float newDistance;
		float3 newNormal;
		if (InteresectSphere(&scene->Hittables[i], ray, minDistance, maxDistance, &newDistance, &newNormal)) {
			if (newDistance < distance) {
				distance = newDistance;
				normal = newNormal;
			}
		}
	}

	if (distance == kInfinity) {
		return false;
	}

	surfaceInteraction->Position = ray.Origin + (ray.Direction * distance);
	surfaceInteraction->Normal = normal;

	return true;
}
