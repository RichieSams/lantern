/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "scene/mesh_elements.h"

#include "bsdfs/bsdf.h"

#include <vector>


namespace Lantern {

void LoadMeshesFromObj(const char *filePath, std::vector<Mesh> &meshes, std::vector<BSDF> &materials);

} // End of namespace Lantern
