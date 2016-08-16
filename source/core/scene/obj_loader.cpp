/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/obj_loader.h"

#include "math/int_types.h"

#include <tiny_obj_loader/tiny_obj_loader.h>


namespace Lantern {

void LoadMeshesFromObj(const char *filePath, std::vector<Mesh> &meshes) {
	std::vector<tinyobj::shape_t> tinyObjShapes;
	std::vector<tinyobj::material_t> tinyObjMaterials;
	std::string err;

	tinyobj::LoadObj(tinyObjShapes, tinyObjMaterials, err, filePath);

	// We ignore materials for now

	// Copy over the vertices and indices
	for (auto &shape : tinyObjShapes) {
		Mesh mesh;
		for (uint i = 0; i < shape.mesh.positions.size(); i += 3) {
			// tiny_obj_loader uses LH coords
			mesh.Positions.emplace_back(shape.mesh.positions[i], shape.mesh.positions[i + 1], shape.mesh.positions[i + 2]);
		}
		for (uint i = 0; i < shape.mesh.normals.size(); i += 3) {
			// tiny_obj_loader uses LH coords
			mesh.Normals.emplace_back(shape.mesh.normals[i], shape.mesh.normals[i + 1], shape.mesh.normals[i + 2]);
		}
		for (uint i = 0; i < shape.mesh.indices.size(); ++i) {
			mesh.Indices.push_back(shape.mesh.indices[i]);
		}

		meshes.push_back(mesh);
	}
}

} // End of namespace Lantern
