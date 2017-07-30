/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2017
*/

#pragma once

#include "materials/textures/texture.h"


namespace Lantern {

class ConstantTexture : public Texture {
public:
	ConstantTexture(float3 value)
		: m_value(value) {
	}

private:
	float3 m_value;

public:
	float3 Sample(float2 texCoord) override {
		return m_value;
	}
};

} // End of namespace Lantern
