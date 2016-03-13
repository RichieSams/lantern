/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/material.h"

#include "math/uniform_sampler.h"

#include <cmath>


namespace Lantern {

class MirrorMaterial : public Material {
public:
	MirrorMaterial(float3 diffuseColor)
		: m_diffuseColor(diffuseColor) {
	}

private:
	float3 m_diffuseColor;

public:
	float3 EmissiveColor() override {
		return float3(0.0f);
	}
	float3 Eval(float3a wi, float3a wo, float3a normal) override {
		return m_diffuseColor;
	}
	/**
	* Reflects the incoming ray as a perfect mirror
	*
	* @param normal     The normal that defines the hemisphere
	*
	* @param sampler    The sampler to use for internal random number generation
	* @return           A cosine weighted random direction in the hemisphere
	*/
	float3a Sample(float3a wi, float3a normal, UniformSampler *sampler, float *pdf) override {
		float3 direction =  wi - 2.0f * normal * dot(normal, wi);
		
		*pdf = 1.0f;
		return direction;
	}
};

} // End of namespace Lantern
