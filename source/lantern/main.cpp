/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "camera/frame_data.h"

#include "visualizer/visualizer.h"

#include "integrator/integrator.h"

#include "argparse.h"

#include <thread>

int main(int argc, const char *argv[]) {
	const uint32_t width = 1280;
	const uint32_t height = 720;

	lantern::FrameData transferFrames[3] = {
	    lantern::FrameData(width, height),
	    lantern::FrameData(width, height),
	    lantern::FrameData(width, height)};
	std::atomic<lantern::FrameData *> swapBuffer(&transferFrames[1]);

	lantern::Visualizer visualizer(&transferFrames[0], &swapBuffer);
	if (!visualizer.Init(1280, 720)) {
		return -1;
	}

	lantern::Integrator integrator(&transferFrames[2], &swapBuffer);

	std::atomic_bool quit(false);
	std::thread rendererThread(
	    [](lantern::Integrator *_integrator, std::atomic_bool *_quit) {
		    while (!_quit->load(std::memory_order_relaxed)) {
			    _integrator->RenderOneFrame();
		    }
	    },
	    &integrator,
	    &quit);

	visualizer.Run();
	visualizer.Shutdown();

	quit.store(true);
	rendererThread.join();
}
