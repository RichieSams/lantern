/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include <xmmintrin.h>
#include <pmmintrin.h>


int main(int argc, const char *argv[]) {
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	Lantern::Renderer renderer(1280, 720);
	renderer.SetScene();

	bool visualizing = true;
	if (visualizing) {
		Lantern::Visualizer visualizer(&renderer);
		visualizer.Run();
	} else {
		// renderer.Run();
	}
}
