/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"
#include "scene/geometry_generator.h"
#include "scene/lambert_material.h"

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include <xmmintrin.h>
#include <pmmintrin.h>


int main(int argc, const char *argv[]) {
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	Lantern::Scene scene;
	Lantern::Mesh mesh;
	Lantern::CreateCube(1.0f, &mesh);
	Lantern::LambertMaterial *material = new Lantern::LambertMaterial(float3(1.0f, 0.0f, 0.0f), float3(0.0f));
	scene.AddMesh(&mesh, material);
	scene.Commit();

	Lantern::Renderer renderer(&scene);
	

	bool visualizing = true;
	if (visualizing) {
		Lantern::Visualizer visualizer(&renderer, &scene);
		visualizer.Run();
	} else {
		// renderer.Run();
	}
}
