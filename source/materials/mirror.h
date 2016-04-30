/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/material.h"

#include "math/uniform_sampler.h"


namespace Lantern {

class MirrorMaterial : public Material {
public:
	MirrorMaterial(float3 albedo)
		: Material(albedo) {
	}

public:
	float3 Eval(float3a &wi, float3a &wo, float3a &normal) const override {
		return m_albedo;
	}

	float3a Sample(float3a &wo, float3a &normal, UniformSampler *sampler) const override {
		return reflect(wo, normal);
	}

	float Pdf(float3a &wi, float3a &normal) const override {
		return 1.0f;
	}
};

} // End of namespace Lantern
