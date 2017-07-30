/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"

#include <vector>
#include <cstdio>


namespace Lantern {

struct LanternModelFile {
	byte VerticesPerPrimative;

	std::vector<float> Positions;
	std::vector<uint32> Indices;
	std::vector<float> Normals;
	std::vector<float> TexCoords;
};

enum class LMFFlags {
	NONE = 0x00,
	HAS_NORMALS = 0x01,
	HAS_TEXCOORDS = 0x02
};

/**
 * Reads a LanternModelFile from a file pointer into the given struct
 *
 * @param file    The file to read from
 * @param lmf     The struct to read into
 */
bool ReadLMF(FILE *file, LanternModelFile *lmf);
/**
* Writes the contents of a LanternModelFile to the given file
*
* @param file    The file to write into
* @param lmf     The LanternModelFile to write
*/
bool WriteLFM(FILE *file, LanternModelFile *lmf);


} // End of namespace Lantern
