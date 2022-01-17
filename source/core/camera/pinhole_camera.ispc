/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "camera/pinhole_camera.h"
#include "camera/pinhole_camera.isph"

varying Ray GenerateCameraRay(uniform PinholeCamera *uniform camera, varying int x, varying int y) {
	// Shoot rays through the center of the pixel (aka 0.5 vs 0.0)
	//
	// First convert x/y to homogenous coordinates
	// x/y go from 0,0 in the top-left to width,height in the bottom-right
	// Homogenous coordinates go from -1,-1 in the bottom-left to 1,1 in the top-right
	float xHomogenous = (((float)x + 0.5f) / (float)camera->Width) - 0.5f;
	// yHomogenous is inverted, because of the direction flip going from screen coordinates to homogenous
	float yHomogenous = -((((float)y + 0.5f) / (float)camera->Height) - 0.5f);

	// We assume a focal plane distance of 1.0f for convenience
	// xViewSpace = focalPlaneDist * xHomogenous * (tan(FOVx) / 2)
	float xViewSpace = xHomogenous * camera->TanFOVXDiv2;
	float yViewSpace = yHomogenous * camera->TanFOVYDiv2;

	float3 direction = {xViewSpace, yViewSpace, -1.0f};

	Ray ret = {{0.0f, 0.0f, 0.0f}, direction};
	return ret;
}
