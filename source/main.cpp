/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"
#include "scene/geometry_generator.h"
#include "scene/obj_loader.h"

#include "bsdfs/lambert_bsdf.h"
#include "bsdfs/mirror_bsdf.h"

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include <xmmintrin.h>
#include <pmmintrin.h>


void SetScene(Lantern::Scene &scene);
void LoadObjScene(Lantern::Scene &scene);
void LoadBallsScene(Lantern::Scene &scene);

int main(int argc, const char *argv[]) {
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	Lantern::Scene scene;
	SetScene(scene);

	Lantern::Renderer renderer(&scene);
	

	bool visualizing = true;
	if (visualizing) {
		Lantern::Visualizer visualizer(&renderer, &scene);
		visualizer.Run();
	} else {
		// renderer.Run();
	}
}

void SetScene(Lantern::Scene &scene) {
	scene.BackgroundColor = float3(0.846f, 0.933f, 0.949f);

	#if 1
		LoadObjScene(scene);
	#else
		LoadBallsScene(scene);
	#endif

	scene.Commit();
}

void LoadObjScene(Lantern::Scene &scene) {
	scene.SetCamera(M_PI_2, 0.0f, 1.25f, 1280.0f, 720.0f);

	Lantern::LambertBSDF *green = new Lantern::LambertBSDF(float3(0.408f, 0.741f, 0.467f));
	
	// Create Buddha
	std::vector<Lantern::Mesh> buddhaMeshes;
	std::vector<Lantern::BSDF> buddhaBSDFs;
	Lantern::LoadMeshesFromObj("buddha.obj", buddhaMeshes, buddhaBSDFs);
	for (auto &mesh : buddhaMeshes) {
		scene.AddMesh(&mesh, green);
	}
}

void LoadBallsScene(Lantern::Scene &scene) {
	scene.SetCamera(M_PI_2, 0.0f, 20.0f, 1280.0f, 720.0f);

	// Create the materials
	Lantern::LambertBSDF *green = new Lantern::LambertBSDF(float3(0.408f, 0.741f, 0.467f));
	Lantern::LambertBSDF *blue = new Lantern::LambertBSDF(float3(0.392f, 0.584f, 0.929f));
	Lantern::LambertBSDF *orange = new Lantern::LambertBSDF(float3(1.0f, 0.498f, 0.314f));
	Lantern::LambertBSDF *white = new Lantern::LambertBSDF(float3(0.0f));
	Lantern::LambertBSDF *gray = new Lantern::LambertBSDF(float3(0.9f, 0.9f, 0.9f));
	Lantern::MirrorBSDF *mirror = new Lantern::MirrorBSDF(float3(0.95f, 0.95f, 0.95f));

	// Create the floor
	Lantern::Mesh floorMesh;
	Lantern::CreateGrid(50.0f, 50.0f, 2u, 2u, &floorMesh);
	Lantern::TranslateMesh(float3(0.0f, -6.0f, 0.0f), &floorMesh);
	scene.AddMesh(&floorMesh, gray);

	// Create the 9 spheres
	Lantern::Mesh sphereMesh;
	Lantern::Mesh *sphere = &sphereMesh;
	Lantern::CreateGeosphere(2.0f, 8u, sphere);

	scene.AddMesh(sphere, white, float3(1.0f), 800.0f);

	Lantern::TranslateMesh(float3(-4.0f, -4.0f, -4.0f), sphere);
	scene.AddMesh(sphere, green);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, 8.0f), sphere);
	scene.AddMesh(sphere, blue);

	Lantern::TranslateMesh(float3(0.0f, 8.0f, 0.0f), sphere);
	scene.AddMesh(sphere, mirror);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, -8.0f), sphere);
	scene.AddMesh(sphere, orange);

	Lantern::TranslateMesh(float3(8.0f, 0.0f, 0.0f), sphere);
	scene.AddMesh(sphere, blue);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, 8.0f), sphere);
	scene.AddMesh(sphere, green);

	Lantern::TranslateMesh(float3(0.0f, -8.0f, 0.0f), sphere);
	scene.AddMesh(sphere, orange);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, -8.0f), sphere);
	scene.AddMesh(sphere, mirror);
}
