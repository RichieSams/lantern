/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "vector_types.h"


namespace Lantern {

typedef float4 Vertex;

struct Triangle {
	int V0, V1, V2;
};

} // End of namespace Lantern
