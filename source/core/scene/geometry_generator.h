/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "scene/mesh_elements.h"


namespace Lantern {

void CreateBox(float width, float height, float depth, Mesh *mesh);
void CreateSphere(float radius, uint sliceCount, uint stackCount, Mesh *mesh);
void CreateGeosphere(float radius, uint numSubdivisions, Mesh *mesh);
void CreateGrid(float width, float depth, uint m, uint n, Mesh *mesh);

inline void ScaleMesh(float scale, Mesh *mesh) {
	for (auto &vertex : mesh->Positions) {
		vertex *= scale;
	}
}

inline void TranslateMesh(float3 position, Mesh *mesh) {
	for (auto &vertex : mesh->Positions) {
		vertex += position;
	}
}

} // End of namespace Lantern
