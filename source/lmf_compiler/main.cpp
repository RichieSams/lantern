/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "io/lantern_model_file.h"

#include "argparse.h"

#include "tiny_obj_loader/tiny_obj_loader.h"

#include <stdio.h>
#include <stdexcept>
#include <algorithm>

struct LMFCompilerOpts {
	const char *InputPath = nullptr;
	const char *OutputPath = nullptr;
};

void ParseCommandLine(int argc, const char *argv[], LMFCompilerOpts *opts);
void ConvertObjToLMF(LMFCompilerOpts *opts);

int main(int argc, const char *argv[]) {
	LMFCompilerOpts options;

	ParseCommandLine(argc, argv, &options);
	ConvertObjToLMF(&options);
}

void ParseCommandLine(int argc, const char *argv[], LMFCompilerOpts *opts) {
	const char *const usage[] = {
		"lmf_compiler [options] [--] <input_file>",
		NULL,
	};

	struct argparse_option parseOptions[] = {
		OPT_HELP(),
		OPT_GROUP("Basic options"),
		OPT_STRING('o', "output", &opts->OutputPath, "Output Path for the .lmf file"),
		OPT_END(),
	};

	argparse argparse;
	argparse_init(&argparse, parseOptions, usage, 0);
	argparse_describe(&argparse, "Converts OBJ files to Lantern Model Files", "Example:\nlmf_compiler -o dragon.lmf dragon.obj");

	argc = argparse_parse(&argparse, argc, argv);


	//parser.add(
	//	"", true, 1, NULL,
	//	"The output file. This can be relative or absolute.\n"
	//	"The file name should have the .lmf extension",
	//	"-o",
	//	"--output"
	//);

	//argparse.flags

	// Check that we have the correct number of arguments
	if (argc != 1) {
		printf("ERROR: Got an unexpected number of arguments.\n\n");
		argparse_usage(&argparse);
		exit(1);
	}

	opts->InputPath = argv[0];
}

void ConvertObjToLMF(LMFCompilerOpts *opts) {
	std::vector<tinyobj::shape_t> tinyObjShapes;
	std::vector<tinyobj::material_t> tinyObjMaterials;
	std::string err;

	if (!tinyobj::LoadObj(tinyObjShapes, tinyObjMaterials, err, opts->InputPath)) {
		printf("Unable to parse obj file\n");
		printf("%s\n", err.c_str());
		return;
	}

	char outputPath[260];
	char const *pos = strrchr(opts->InputPath, '.');
	if (pos != nullptr) {
		size_t strLen = pos - opts->InputPath;
		memcpy(outputPath, opts->InputPath, strLen);
		outputPath[strLen] = '\0';
	} else {
		strncpy(outputPath, opts->InputPath, sizeof(outputPath));
	}

	// Write out one lmf file per shape
	for (auto &shape : tinyObjShapes) {
		// Check that the mesh is valid
		if (shape.mesh.positions.size() == 0 || shape.mesh.indices.size() == 0) {
			printf("Mesh [%s] does not have positions or indices\n", shape.name.c_str());
			return;
		}

		std::string sanitizedName = shape.name;
		for (auto iter = sanitizedName.begin(); iter != sanitizedName.end(); ++iter) {
			if (*iter == ':' || *iter == '\\' || *iter == '/' || *iter == '*' || *iter == '?' || *iter == '<' || *iter == '>' || *iter == '|' || *iter == '"') {
				*iter = '-';
			}
		}

		// Create a unique name
		char uniqueName[260];
		snprintf(uniqueName, sizeof(uniqueName), "%s%s.lmf", outputPath, sanitizedName.c_str());

		// Create the file
		FILE *file = fopen(uniqueName, "wb");
		if (!file) {
			printf("Unable to open \"%s\" for writing\n",uniqueName);
			return;
		}


		// Convert to LMF data
		Lantern::LanternModelFile lmf;
		
		byte verticesPerPrimitive = shape.mesh.num_vertices[0];
		for (std::size_t i = 1; i < shape.mesh.num_vertices.size(); ++i) {
			if (shape.mesh.num_vertices[i] != verticesPerPrimitive) {
				printf("The number of vertices per primitive is not consistent across [%s]\nThe first face has %u vertices. Found %u vertices for face #%zu\n", shape.name.c_str(), verticesPerPrimitive, shape.mesh.num_vertices[i], i / verticesPerPrimitive);
				return;
			}
		}
		lmf.VerticesPerPrimitive = verticesPerPrimitive;
		
		lmf.Positions = std::move(shape.mesh.positions);
		lmf.Indices = std::move(shape.mesh.indices);
		lmf.Normals = std::move(shape.mesh.normals);
		lmf.TexCoords = std::move(shape.mesh.texcoords);

		// Write the file
		if (!Lantern::WriteLFM(file, &lmf)) {
			printf("LMF write failed");
		}

		// Cleanup
		fclose(file);
	}
}
