/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "linalg.h"
using namespace linalg::aliases;

#include <vector>


namespace Lantern {

struct Mesh {
	std::vector<float3> Positions;
	std::vector<float3> Normals;
	std::vector<float3> Tangents;
	std::vector<float2> TexCoords;
	std::vector<int> Indices;
};

} // End of namespace Lantern
