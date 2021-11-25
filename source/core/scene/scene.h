/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "scene/sphere.h"

namespace lantern {

struct SurfaceInteraction;

class Scene {
public:
	Scene(Sphere *hittables, size_t hittablesLen)
	        : m_hittables(hittables), m_hittablesLen(hittablesLen) {
	}

private:
	Sphere *m_hittables;
	size_t m_hittablesLen;

public:
	bool Interesect(Ray &ray, float minDistance, float maxDistance, SurfaceInteraction *surfaceInteraction);
};

} // End of namespace lantern
