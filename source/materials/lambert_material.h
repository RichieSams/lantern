/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/material.h"

#include "math/sampling.h"


namespace Lantern {

class LambertMaterial : public Material {
public:
	LambertMaterial(float3 albedo)
		: Material(albedo) {
	}

public:
	float3 Eval(float3a &wi, float3a &wo, float3a &normal) const override {
		return m_albedo * M_1_PI * dot(wi, normal);
	}
	
	float3a Sample(float3a &wo, float3a &normal, UniformSampler *sampler) const override {
		return CosineSampleHemisphere(normal, sampler);
	}

	float Pdf(float3a &wi, float3a &normal) const override {
		return dot(wi, normal) * M_1_PI;
	}
};

} // End of namespace Lantern
