/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "scene/sphere.h"

#include "math/ray.h"

namespace lantern {

bool Sphere::Interesect(Ray &ray, float minDistance, float maxDistance, float *distance, float3 *normal) {
	float3 oc = ray.Origin - Origin;
	float a = length2(ray.Direction);
	float halfB = dot(oc, ray.Direction);
	float c = length2(oc) - RadiusSquared;

	float discriminant = halfB * halfB - a * c;
	if (discriminant < 0) {
		return false;
	}

	float sqrtDiscrim = sqrtf(discriminant);

	// Find th enearest root that lies in the acceptable range
	float root = (-halfB - sqrtDiscrim) / a;
	if (root < minDistance || root > maxDistance) {
		root = (-halfB + sqrtDiscrim) / a;
		if (root < minDistance || root > maxDistance) {
			return false;
		}
	}

	*distance = root;
	float3 position = ray.Origin + (ray.Direction * root);
	*normal = normalize(position - Origin);

	return true;
}

} // End of namespace lantern
