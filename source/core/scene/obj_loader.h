/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "scene/mesh_elements.h"

#include <vector>


namespace Lantern {

void LoadMeshesFromObj(const char *filePath, std::vector<Mesh> &meshes);

} // End of namespace Lantern
