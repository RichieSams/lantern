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
	Lantern::Mesh cube;
	Lantern::CreateBox(4.0f, 4.0f, 4.0f, &cube);
	Lantern::TranslateMesh(float3(0.0f, -2.0f, 0.0f), &cube);

	Lantern::Mesh sphere;
	Lantern::CreateGeosphere(1.0f, 3u, &sphere);
	Lantern::TranslateMesh(float3(0.0f, 1.0f, 0.0f), &sphere);

	Lantern::LambertMaterial *grayMaterial = new Lantern::LambertMaterial(float3(0.5f, 0.5f, 0.5f), float3(0.0f));
	scene.AddMesh(&cube, grayMaterial);
	scene.AddMesh(&sphere, grayMaterial);
	scene.Commit();

	Lantern::Renderer renderer(&scene);
	

	bool visualizing = true;
	if (visualizing) {
		Lantern::Visualizer visualizer(&renderer, &scene);
		visualizer.Run();
	} else {
		// renderer.Run();
	}

	delete grayMaterial;
}
