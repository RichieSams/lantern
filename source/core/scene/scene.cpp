/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "scene/scene.h"

#include "integrator/surface_interaction.h"

#include "math/constants.h"
#include "math/ray.h"

#include <string.h>

namespace lantern {

bool Scene::Interesect(Ray &ray, float minDistance, float maxDistance, SurfaceInteraction *surfaceInteraction) {
	float distance = kInfinity;
	float3 normal;

	for (size_t i = 0; i < m_hittablesLen; ++i) {
		float newDistance;
		float3 newNormal;
		if (m_hittables[i].Interesect(ray, minDistance, maxDistance, &newDistance, &newNormal)) {
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

} // End of namespace lantern