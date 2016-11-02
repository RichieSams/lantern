/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"

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
	if (!scene.LoadSceneFromJSON("scene.json")) {
		std::printf("Could not load scene.json\n");
		return 1;
	}
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
