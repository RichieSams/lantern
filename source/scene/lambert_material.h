/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "scene/material.h"

#include "math/uniform_sampler.h"

#include <cmath>


namespace Lantern {

class LambertMaterial : public Material {
public:
	LambertMaterial(float3 diffuseColor, float3 emissiveColor)
		: m_diffuseColor(diffuseColor),
		  m_emissiveColor(emissiveColor) {
	}

private:
	float3 m_diffuseColor;
	float3 m_emissiveColor;

public:
	float3 EmissiveColor() override { return m_emissiveColor; }
	float3 Eval(float3a wi, float3a wo, float3a normal) override {
		return m_diffuseColor * dot(wi, normal) * M_1_PI;
	}
	/**
	* Creates a random direction in the hemisphere defined by the normal, weighted by a cosine lobe
	*
	* Based on http://www.rorydriscoll.com/2009/01/07/better-sampling/
	*
	* @param wi         The direction of the incoming light
	* @param normal     The normal that defines the hemisphere
	*
	* @param sampler    The sampler to use for internal random number generation
	* @return           A cosine weighted random direction in the hemisphere
	*/
	float3a Sample(float3a wi, float3a normal, UniformSampler *sampler, float *pdf) override {
		// Create random coordinates in the local coordinate system
		float rand = sampler->NextFloat();
		float r = std::sqrtf(rand);
		float theta = sampler->NextFloat() * 6.28318530718f /* 2 PI */;

		float x = r * std::cosf(theta);
		float y = r * std::sinf(theta);
		float z = std::sqrtf(std::fmax(0.0f, 1.0f - x * x - y * y));

		// Find an axis that is not parallel to normal
		float3 majorAxis;
		if (abs(normal.x) < 0.57735026919f /* 1 / sqrt(3) */) {
			majorAxis = float3(1, 0, 0);
		} else if (abs(normal.y) < 0.57735026919f /* 1 / sqrt(3) */) {
			majorAxis = float3(0, 1, 0);
		} else {
			majorAxis = float3(0, 0, 1);
		}

		// Use majorAxis to create a coordinate system relative to world space
		float3 u = normalize(cross(normal, majorAxis));
		float3 v = cross(normal, u);
		float3 w = normal;


		// Transform from local coordinates to world coordinates
		float3 direction =  normalize(u * x +
		                              v * y +
		                              w * z);

		*pdf = dot(direction, normal) * M_1_PI;
		return direction;
	}
};

} // End of namespace Lantern
