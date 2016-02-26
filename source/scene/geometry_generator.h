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

inline void ScaleMesh(float scale, Mesh *mesh) {
	for (auto &vertex : mesh->Vertices) {
		vertex *= scale;
	}
}

inline void TranslateMesh(float3 position, Mesh *mesh) {
	float4 translation(position, 1.0f);

	for (auto &vertex : mesh->Vertices) {
		vertex += translation;
	}
}

} // End of namespace Lantern
