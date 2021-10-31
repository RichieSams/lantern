/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "render_host/presentation_buffer.h"
#include "render_host/render_host.h"

#include "visualizer/visualizer.h"

#include "integrator/integrator.h"

#include "argparse.h"

#include <thread>

int main(int argc, const char *argv[]) {
	const uint32_t width = 1280;
	const uint32_t height = 720;

	lantern::PresentationBuffer transferFrames[3] = {
	    lantern::PresentationBuffer(width, height),
	    lantern::PresentationBuffer(width, height),
	    lantern::PresentationBuffer(width, height)};
	std::atomic<lantern::PresentationBuffer *> swapBuffer(&transferFrames[1]);

	lantern::Integrator integrator(width, height);

	lantern::RenderHost renderHost(&integrator, &transferFrames[0], &swapBuffer);

	lantern::Visualizer visualizer(&transferFrames[2], &renderHost.GenerationNumber, &swapBuffer);
	if (!visualizer.Init(width, height)) {
		return -1;
	}

	std::atomic_bool quit(false);
	std::thread rendererThread(
	    [&renderHost, &quit]() {
		    renderHost.Run(&quit);
	    });

	visualizer.Run();
	visualizer.Shutdown();

	quit.store(true);
	rendererThread.join();
}
