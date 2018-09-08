/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include "argparse.h"

#include <xmmintrin.h>
#include <pmmintrin.h>

#include <thread>


int main(int argc, const char *argv[]) {
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	struct LanternOpts {
		const char *ScenePath = "scene.json";
		bool Verbose = false;
	} options;

	const char *const usage[] = {
		"lantern [options] [[--] args]",
		"lantern [options]",
		NULL,
	};
	
	struct argparse_option parseOptions[] = {
		OPT_HELP(),
		OPT_GROUP("Root options"),
		OPT_BOOLEAN('v', "verbose", &options.Verbose, "Use verbose logging"),
		OPT_GROUP("Basic Options"),
		OPT_STRING('s', "scene", &options.ScenePath, "Path to the scene.json file. If ommited, Lantern will search for 'scene.json' in the working directory"),
		OPT_END(),
	};

	argparse argparse;
	argparse_init(&argparse, parseOptions, usage, 0);
	argparse_describe(&argparse, "Renders a scene with an interactive preview", "");

	argc = argparse_parse(&argparse, argc, argv);

	// Load the scene
	Lantern::Scene scene;
	if (!scene.LoadSceneFromJSON(options.ScenePath)) {
		printf("Could not load scene.json\n");
		return 1;
	}

	Lantern::FrameBuffer transferFrames[3] = {
		Lantern::FrameBuffer(scene.Camera->FrameBufferWidth, scene.Camera->FrameBufferHeight),
		Lantern::FrameBuffer(scene.Camera->FrameBufferWidth, scene.Camera->FrameBufferHeight),
		Lantern::FrameBuffer(scene.Camera->FrameBufferWidth, scene.Camera->FrameBufferHeight)
	};
	std::atomic<Lantern::FrameBuffer *> swapBuffer(&transferFrames[1]);

	Lantern::Renderer renderer(&scene, &transferFrames[0], &swapBuffer);
	Lantern::Visualizer visualizer(&scene, &transferFrames[2], &swapBuffer);
	
	std::atomic_bool quit(false);
	std::thread rendererThread(
		[](Lantern::Renderer *renderer, std::atomic_bool *quit) {
			while (!quit->load(std::memory_order_relaxed)) {
				renderer->RenderFrame();
			}
	}, &renderer, &quit);

	visualizer.Run();

	quit.store(true);
	rendererThread.join();
}
