/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "io/lantern_model_file.h"

#include <ez_option_parser.h>

#include <tiny_obj_loader/tiny_obj_loader.h>

#include <cstdio>
#include <stdexcept>


void PrintHelpString(ez::ezOptionParser &parser) {
	std::string usage;
	parser.getUsage(usage);
	std::cout << usage;
}

struct CMDLineArgs {
	std::string inputFile;
	std::string outputFile;
};

void ParseCommandLine(int argc, const char *argv[], CMDLineArgs *args);
void ConvertObjToLMF(CMDLineArgs *args);

int main(int argc, const char *argv[]) {
	CMDLineArgs args;
	ParseCommandLine(argc, argv, &args);
	ConvertObjToLMF(&args);
}

void ParseCommandLine(int argc, const char *argv[], CMDLineArgs *args) {
	ez::ezOptionParser parser;
	parser.overview = "Converts OBJ files to Lantern Model Files";
	parser.syntax = "lmf_compiler [OPTIONS] <input obj file>";
	parser.example = "lmf_compiler -o dragon.lmf dragon.obj\n\n";

	// Help message
	parser.add(
		"", false, 1, NULL,
		"Display usage instructions",
		"-h",
		"--help"
	);

	parser.add(
		"", true, 1, NULL,
		"The output file. This can be relative or absolute.\n"
		"The file name should have the .lmf extension",
		"-o",
		"--output"
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

	// Check that we have the correct number of arguments
	if (parser.lastArgs.size() != 1) {
		std::cerr << "ERROR: Got an unexpected number of arguments.\n\n";
		PrintHelpString(parser);
		exit(1);
	}

	// Load the data
	args->inputFile = *parser.lastArgs[0];
	parser.get("-o")->getString(args->outputFile);
}

void ConvertObjToLMF(CMDLineArgs *args) {
	std::vector<tinyobj::shape_t> tinyObjShapes;
	std::vector<tinyobj::material_t> tinyObjMaterials;
	std::string err;

	if (!tinyobj::LoadObj(tinyObjShapes, tinyObjMaterials, err, args->inputFile.c_str())) {
		printf("Unable to parse obj file\n");
		printf("%s\n", err.c_str());
		return;
	}

	// Strip off the extension
	std::string outputFile;
	std::size_t pos = args->outputFile.find_last_of('.');
	if (pos != std::string::npos) {
		outputFile = args->outputFile.substr(0, pos);
	} else {
		outputFile = args->outputFile;
	}

	// Write out one lmf file per shape
	for (auto &shape : tinyObjShapes) {
		// Check that the mesh is valid
		if (shape.mesh.positions.size() == 0 || shape.mesh.indices.size() == 0) {
			printf("Mesh [%s] does not have positions or indices\n", shape.name.c_str());
			return;
		}

		// Create a unique name
		std::stringstream strStream;
		strStream << outputFile << shape.name << ".lmf";

		// Create the file
		FILE *file = fopen(strStream.str().c_str(), "wb");
		if (!file) {
			printf("Unable to open \"%s\" for writing\n", strStream.str().c_str());
			return;
		}


		// Convert to LMF data
		Lantern::LanternModelFile lmf;
		
		byte verticesPerPrimative = shape.mesh.num_vertices[0];
		for (std::size_t i = 1; i < shape.mesh.num_vertices.size(); ++i) {
			if (shape.mesh.num_vertices[i] != verticesPerPrimative) {
				printf("The number of vertices per primative is not consistent across [%s]\nThe first face has %u vertices. Found %u vertices for face #%llu\n", shape.name.c_str(), verticesPerPrimative, shape.mesh.num_vertices[i], i / verticesPerPrimative);
				return;
			}
		}
		lmf.VerticesPerPrimative = verticesPerPrimative;
		
		lmf.Positions = std::move(shape.mesh.positions);
		lmf.Indices = std::move(shape.mesh.indices);
		lmf.Normals = std::move(shape.mesh.normals);
		lmf.TexCoords = std::move(shape.mesh.normals);

		// Write the file
		if (!Lantern::WriteLFM(file, &lmf)) {
			printf("LMF write failed");
		}

		// Cleanup
		fclose(file);
	}
}
