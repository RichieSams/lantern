/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "scene/sphere.h"
#include "scene/sphere.isph"

#include "math/ray.isph"

varying bool InteresectSphere(uniform Sphere *uniform sphere, varying Ray &ray, varying float minDistance, varying float maxDistance, varying float *uniform distance, varying float3 *uniform normal) {
	float3 oc = ray.Origin - sphere->Origin;
	float a = length2(ray.Direction);
	float halfB = dot(oc, ray.Direction);
	float c = length2(oc) - sphere->RadiusSquared;

	float discriminant = halfB * halfB - a * c;
	if (discriminant < 0) {
		return false;
	}

	float sqrtDiscrim = sqrt(discriminant);

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
	*normal = normalize(position - sphere->Origin);

	return true;
}
