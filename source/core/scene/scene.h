/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "scene/sphere.h"

struct SurfaceInteraction;
struct PinholeCamera;

extern "C" {

struct Scene {
	Sphere *Hittables;
	size_t HittablesLen;

	PinholeCamera *Camera;
};
}
