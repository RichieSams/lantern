/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"
#include "scene/geometry_generator.h"
#include "scene/obj_loader.h"

#include "materials/material.h"
#include "materials/bsdfs/lambert_bsdf.h"
#include "materials/bsdfs/mirror_bsdf.h"
#include "materials/bsdfs/ideal_specular_dielectric.h"
#include "materials/media/non_scattering_medium.h"
#include "materials/media/isotropic_scattering_medium.h"

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
	scene.SetCamera(1.25f, -M_PI_2, 2.0f, 1280.0f, 720.0f);

	Lantern::IdealSpecularDielectric *glass = new Lantern::IdealSpecularDielectric(float3(1.0f), 1.35f);
	Lantern::NonScatteringMedium *redTransmission = new Lantern::NonScatteringMedium(float3(0.9801986733f, 0.00609674656f, 0.00334596545f), 1.0f);
	Lantern::IsotropicScatteringMedium *redScattering = new Lantern::IsotropicScatteringMedium(float3(0.9801986733f, 0.00609674656f, 0.00334596545f), 1.0f, 0.01f);

	Lantern::Material *material = new Lantern::Material(glass, redScattering);

	// Create Dragon
	std::vector<Lantern::Mesh> dragonMeshes;
	Lantern::LoadMeshesFromObj("dragon.obj", dragonMeshes);
	for (auto &mesh : dragonMeshes) {
		scene.AddMesh(&mesh, material);
	}
}

void LoadBallsScene(Lantern::Scene &scene) {
	scene.SetCamera(M_PI_2, 0.0f, 40.0f, 1280.0f, 720.0f);

	// Create the materials
	Lantern::Material *green = new Lantern::Material(new Lantern::LambertBSDF(float3(0.408f, 0.741f, 0.467f)), nullptr);
	Lantern::Material *blue = new Lantern::Material(new Lantern::LambertBSDF(float3(0.392f, 0.584f, 0.929f)), nullptr);
	Lantern::Material *orange = new Lantern::Material(new Lantern::LambertBSDF(float3(1.0f, 0.498f, 0.314f)), nullptr);
	Lantern::Material *black = new Lantern::Material(new Lantern::LambertBSDF(float3(0.0f)), nullptr);
	Lantern::Material *gray = new Lantern::Material(new Lantern::LambertBSDF(float3(0.9f, 0.9f, 0.9f)), nullptr);
	Lantern::Material *mirror = new Lantern::Material(new Lantern::MirrorBSDF(float3(0.95f, 0.95f, 0.95f)), nullptr);
	Lantern::Material *glass = new Lantern::Material(new Lantern::IdealSpecularDielectric(float3(0.95f), 1.5f), nullptr);

	// Create the floor
	Lantern::Mesh floorMesh;
	Lantern::CreateGrid(50.0f, 50.0f, 2u, 2u, &floorMesh);
	Lantern::TranslateMesh(float3(0.0f, -6.0f, 0.0f), &floorMesh);
	scene.AddMesh(&floorMesh, gray);

	// Create the 9 spheres
	Lantern::Mesh sphereMesh;
	Lantern::Mesh *sphere = &sphereMesh;
	Lantern::CreateGeosphere(2.0f, 3u, sphere);

	scene.AddMesh(sphere, black, float3(1.0f), 800.0f);

	Lantern::TranslateMesh(float3(-4.0f, -4.0f, -4.0f), sphere);
	scene.AddMesh(sphere, mirror);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, 8.0f), sphere);
	scene.AddMesh(sphere, blue);

	Lantern::TranslateMesh(float3(0.0f, 8.0f, 0.0f), sphere);
	scene.AddMesh(sphere, glass);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, -8.0f), sphere);
	scene.AddMesh(sphere, orange);

	Lantern::TranslateMesh(float3(8.0f, 0.0f, 0.0f), sphere);
	scene.AddMesh(sphere, blue);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, 8.0f), sphere);
	scene.AddMesh(sphere, green);

	Lantern::TranslateMesh(float3(0.0f, -8.0f, 0.0f), sphere);
	scene.AddMesh(sphere, mirror);

	Lantern::TranslateMesh(float3(0.0f, 0.0f, -8.0f), sphere);
	scene.AddMesh(sphere, glass);
}
