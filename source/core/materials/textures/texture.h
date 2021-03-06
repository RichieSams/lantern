/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2017
*/

#pragma once

#include "math/vector_types.h"


namespace Lantern {

class Scene;

class Texture {
public:
	virtual ~Texture() = default;

public:
	virtual float3 Sample(float2 texCoord) = 0;
};

} // End of namespace Lantern
