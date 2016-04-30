/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"
#include "scene/geometry_generator.h"

#include "materials/lambert_material.h"
#include "materials/mirror.h"

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include <xmmintrin.h>
#include <pmmintrin.h>


int main(int argc, const char *argv[]) {
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	Lantern::Scene scene;
	scene.SetCamera(M_PI_2, 0.0f, 20.0f, 1280.0f, 720.0f);

	// Create the materials
	Lantern::LambertMaterial gray(float3(0.9f, 0.9f, 0.9f));
	Lantern::LambertMaterial green(float3(0.408f, 0.741f, 0.467f));
	Lantern::LambertMaterial blue(float3(0.392f, 0.584f, 0.929f));
	Lantern::LambertMaterial orange(float3(1.0f, 0.498f, 0.314f));
	Lantern::LambertMaterial white(float3(1.0f));
	Lantern::LambertMaterial black(float3(0.0f));
	Lantern::MirrorMaterial mirror(float3(0.95f, 0.95f, 0.95f));

	// Create the floor
	Lantern::Mesh floorMesh;
	Lantern::CreateGrid(50.0f, 50.0f, 2u, 2u, &floorMesh);
	Lantern::TranslateMesh(float3(0.0f, -6.0f, 0.0f), &floorMesh);
	scene.AddMesh(&floorMesh, &gray);

	// Create the 9 spheres
	Lantern::Mesh sphere;
	Lantern::CreateGeosphere(2.0f, 2u, &sphere);

	Lantern::TranslateMesh(float3(0.0f, 20.0f, 0.0f), &sphere);
	scene.AddMesh(&sphere, &white, float3(1.0f), 8000.0f);

	Lantern::TranslateMesh(float3(-4.0f, -24.0f, -4.0f), &sphere);
	scene.AddMesh(&sphere, &green);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, 8.0f), &sphere);
	scene.AddMesh(&sphere, &blue);

	Lantern::TranslateMesh(float3(0.0f, 8.0f, 0.0f), &sphere);
	scene.AddMesh(&sphere, &gray);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, -8.0f), &sphere);
	scene.AddMesh(&sphere, &orange);

	Lantern::TranslateMesh(float3(8.0f, 0.0f, 0.0f), &sphere);
	scene.AddMesh(&sphere, &blue);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, 8.0f), &sphere);
	scene.AddMesh(&sphere, &green);

	Lantern::TranslateMesh(float3(0.0f, -8.0f, 0.0f), &sphere);
	scene.AddMesh(&sphere, &orange);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, -8.0f), &sphere);
	scene.AddMesh(&sphere, &gray);

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
