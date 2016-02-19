/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "scene/mesh_elements.h"

#include "par_shapes.h"


namespace Lantern {

inline void CreateSphere(float radius, float3 position, Mesh *mesh) {
	par_shapes_mesh *parMesh = par_shapes_create_subdivided_sphere(5);
	par_shapes_scale(parMesh, radius, radius, radius);
	par_shapes_translate(parMesh, position.x, position.y, position.z);

	mesh->Vertices.assign((Vertex *)parMesh->points, (Vertex *)parMesh->points + (parMesh->npoints / 3));
	mesh->Triangles.assign((Triangle *)parMesh->triangles, (Triangle *)parMesh->triangles + (parMesh->ntriangles / 3));

	par_shapes_free_mesh(parMesh);
}

inline void CreateCube(float size, Mesh *mesh) {
	mesh->Vertices.resize(8);
	mesh->Vertices[0].x = -size; mesh->Vertices[0].y = -size; mesh->Vertices[0].z = -size;
	mesh->Vertices[1].x = -size; mesh->Vertices[1].y = -size; mesh->Vertices[1].z = +size;
	mesh->Vertices[2].x = -size; mesh->Vertices[2].y = +size; mesh->Vertices[2].z = -size;
	mesh->Vertices[3].x = -size; mesh->Vertices[3].y = +size; mesh->Vertices[3].z = +size;
	mesh->Vertices[4].x = +size; mesh->Vertices[4].y = -size; mesh->Vertices[4].z = -size;
	mesh->Vertices[5].x = +size; mesh->Vertices[5].y = -size; mesh->Vertices[5].z = +size;
	mesh->Vertices[6].x = +size; mesh->Vertices[6].y = +size; mesh->Vertices[6].z = -size;
	mesh->Vertices[7].x = +size; mesh->Vertices[7].y = +size; mesh->Vertices[7].z = +size;

	mesh->Triangles.resize(12);
	// left side
	mesh->Triangles[0].V0 = 0; mesh->Triangles[0].V1 = 2; mesh->Triangles[0].V2 = 1;
	mesh->Triangles[1].V0 = 1; mesh->Triangles[1].V1 = 2; mesh->Triangles[1].V2 = 3;

	// right side
	mesh->Triangles[2].V0 = 4; mesh->Triangles[2].V1 = 5; mesh->Triangles[2].V2 = 6;
	mesh->Triangles[3].V0 = 5; mesh->Triangles[3].V1 = 7; mesh->Triangles[3].V2 = 6;

	// bottom side
	mesh->Triangles[4].V0 = 0; mesh->Triangles[4].V1 = 1; mesh->Triangles[4].V2 = 4;
	mesh->Triangles[5].V0 = 1; mesh->Triangles[5].V1 = 5; mesh->Triangles[5].V2 = 4;

	// top side
	mesh->Triangles[6].V0 = 2; mesh->Triangles[6].V1 = 6; mesh->Triangles[6].V2 = 3;
	mesh->Triangles[7].V0 = 3; mesh->Triangles[7].V1 = 6; mesh->Triangles[7].V2 = 7;

	// front side
	mesh->Triangles[8].V0 = 0; mesh->Triangles[8].V1 = 4; mesh->Triangles[8].V2 = 2;
	mesh->Triangles[9].V0 = 2; mesh->Triangles[9].V1 = 4; mesh->Triangles[9].V2 = 6;

	// back side
	mesh->Triangles[10].V0 = 1; mesh->Triangles[10].V1 = 3; mesh->Triangles[10].V2 = 5;
	mesh->Triangles[11].V0 = 3; mesh->Triangles[11].V1 = 7; mesh->Triangles[11].V2 = 5;
}

} // End of namespace Lantern
