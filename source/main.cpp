/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "common/global_args.h"

#include "graphics/atomic_frame_buffer.h"

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <thread>


void StartRendererWrapper(Lantern::Renderer *renderer) {
	renderer->Start();
}


int main(int argc, const char *argv[]) {
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	Lantern::GlobalArgs globalArgs;
	Lantern::AtomicFrameBuffer frameBuffer(1280, 720);
	globalArgs.FrameBuffer = &frameBuffer;
	globalArgs.RenderChanged.store(false);
	
	Lantern::Renderer renderer(&globalArgs);
	std::thread renderThread(StartRendererWrapper, &renderer);

	Lantern::Visualizer visualizer(&globalArgs);
	visualizer.Run();

	renderThread.join();
}
