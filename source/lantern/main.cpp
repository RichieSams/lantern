/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/scene.h"

#include "visualizer/visualizer.h"

#include "renderer/renderer.h"

#include <ez_option_parser.h>

#include <xmmintrin.h>
#include <pmmintrin.h>


void PrintHelpString(ez::ezOptionParser &parser) {
	std::string usage;
	parser.getUsage(usage);
	printf("%s", usage.c_str());
}

int main(int argc, const char *argv[]) {
	_MM_SET_FLUSH_ZERO_MODE(_MM_FLUSH_ZERO_ON);
	_MM_SET_DENORMALS_ZERO_MODE(_MM_DENORMALS_ZERO_ON);

	ez::ezOptionParser parser;
	parser.overview = "Renders a scene with an interactive preview";
	parser.syntax = "lantern [OPTIONS]";
	parser.example = "lantern -s dragon_scene.json\n\n";

	// Help message
	parser.add(
		"", false, 1, NULL,
		"Display usage instructions",
		"-h",
		"--help"
	);

	parser.add(
		"", false, 1, NULL,
		"The path to the scene JSON file\n"
		"This defaults to 'scene.json'",
		"-s",
		"--scene"
	);

	parser.parse(argc, argv);

	if (parser.isSet("-h")) {
		PrintHelpString(parser);
		exit(1);
	}

	// Check that the options are valid
	std::vector<std::string> badOptions;
	if (!parser.gotRequired(badOptions)) {
		for (std::size_t i = 0; i < badOptions.size(); ++i) {
			std::cerr << "ERROR: Missing required option " << badOptions[i] << ".\n\n";
		}
		PrintHelpString(parser);
		exit(1);
	}
	badOptions.clear();

	if (!parser.gotExpected(badOptions)) {
		for (std::size_t i = 0; i < badOptions.size(); ++i)
			std::cerr << "ERROR: Got an unexpected number of arguments for option " << badOptions[i] << ".\n\n";

		PrintHelpString(parser);
		exit(1);
	}

	// Load the args
	std::string scenePath("scene.json");
	if (parser.isSet("-s")) {
		parser.get("-s")->getString(scenePath);
	}

	// Load the scene
	Lantern::Scene scene;
	if (!scene.LoadSceneFromJSON(scenePath)) {
		printf("Could not load scene.json\n");
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
