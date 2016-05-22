/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "materials/material.h"

#include "math/uniform_sampler.h"
#include "math/float_math.h"


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

	float Pdf(float3a &wi, float3a &wo, float3a &normal) const override {
		float3a reflection = reflect(wi, normal);
		return FloatNearlyEqual(reflection.x, wo.x) && FloatNearlyEqual(reflection.y, wo.y) && FloatNearlyEqual(reflection.z, wo.z) ? 1.0f : 0.0f;
	}
};

} // End of namespace Lantern
