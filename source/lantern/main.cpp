/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "visualizer/visualizer.h"

#include "argparse.h"

int main(int argc, const char *argv[]) {
	Lantern::Visualizer visualizer;
	if (!visualizer.Init(1280, 720)) {
		return 1;
	}

	visualizer.Run();
	visualizer.Shutdown();
}
