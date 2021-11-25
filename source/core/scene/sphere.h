/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "linalg.h"
using namespace linalg::aliases;

namespace lantern {

struct Ray;

struct Sphere {
public:
	Sphere(float3 origin, float radius)
	        : Origin(origin),
	          RadiusSquared(radius * radius) {
	}

public:
	float3 Origin;
	float RadiusSquared;

public:
	bool Interesect(Ray &ray, float minDistance, float maxDistance, float *distance, float3 *normal);
};

} // End of namespace lantern
