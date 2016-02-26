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

struct Mesh {
	std::vector<Vertex> Vertices;
	std::vector<float3> Normals;
	std::vector<float3> Tangents;
	std::vector<float2> TexCoords;
	std::vector<int> Indices;
};

} // End of namespace Lantern
