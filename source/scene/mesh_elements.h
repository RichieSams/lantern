/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"

#include <vector>


namespace Lantern {

typedef float4 Vertex;

struct Triangle {
	int V0, V1, V2;
};

struct Mesh {
	std::vector<Vertex> Vertices;
	std::vector<Triangle> Triangles;
};

} // End of namespace Lantern
