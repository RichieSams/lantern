/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include "math/constants.h"
#include "math/ray.h"

#include "linalg.h"
using namespace linalg::aliases;

#include <inttypes.h>

namespace lantern {

class PinholeCamera {
public:
	PinholeCamera(uint32_t width, uint32_t height, float horizontalFOV)
	        : m_width(width),
	          m_height(height),
	          m_tanFOVXDiv2(tanf(horizontalFOV * kPiOver180 * 0.5f)),
	          m_tanFOVYDiv2(tanf(horizontalFOV / (float)width * (float)height * kPiOver180 * 0.5f)) {
	}

private:
	uint32_t m_width;
	uint32_t m_height;
	float m_tanFOVXDiv2;
	float m_tanFOVYDiv2;

public:
	Ray GetRay(uint32_t x, uint32_t y) {
		// Shoot rays through the center of the pixel (aka 0.5 vs 0.0)
		//
		// First convert x/y to homogenous coordinates
		// x/y go from 0,0 in the top-left to width,height in the bottom-right
		// Homogenous coordinates go from -1,-1 in the bottom-left to 1,1 in the top-right
		float xHomogenous = (((float)x + 0.5f) / (float)m_width) - 0.5f;
		// yHomogenous is inverted, because of the direction flip going from screen coordinates to homogenous
		float yHomogenous = -((((float)y + 0.5f) / (float)m_height) - 0.5f);

		// We assume a focal plane distance of 1.0f for convenience
		// xViewSpace = focalPlaneDist * xHomogenous * (tan(FOVx) / 2)
		float xViewSpace = xHomogenous * m_tanFOVXDiv2;
		float yViewSpace = yHomogenous * m_tanFOVYDiv2;

		float3 direction(xViewSpace, yViewSpace, -1.0f);

		return Ray(float3(0.0f, 0.0f, 0.0f), direction);
	}
};

} // namespace lantern
