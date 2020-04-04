/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "io/lantern_model_file.h"

#include "io/file_io.h"


namespace Lantern {

/*
	struct LanternModelFile_FileFormat {
		uint32 Magic;
		byte VerticesPerPrimitive;
		uint32 Flags;

		uint64 NumPositions;
		std::vector<float> Positions;
		uint64 NumIndices;
		std::vector<float> Indices;

		uint64 NumNormals;
		std::vector<float> Normals;

		uint64 NumTexCoords;
		std::vector<float> TexCoords;
	};
*/

bool ReadLMF(FILE *file, LanternModelFile *lmf) {
	uint32 magic = ReadUInt32(file);
	if (!VerifyMagicNumber(magic, 'L', 'M', 'F', '\0')) {
		return false;
	}

	// Read the rest of the header
	lmf->VerticesPerPrimitive = ReadByte(file);
	uint32 flags = ReadUInt32(file);

	// Read the main data
	uint64 numPositions = ReadUInt64(file);
	lmf->Positions.resize(numPositions * 3);
	Read(file, &lmf->Positions[0], numPositions * sizeof(float) * 3);

	uint64 numIndices = ReadUInt64(file);
	lmf->Indices.resize(numIndices);
	Read(file, &lmf->Indices[0], numIndices * sizeof(uint32));

	// Read the optional data
	if ((flags & (uint32)LMFFlags::HAS_NORMALS) == (uint32)LMFFlags::HAS_NORMALS) {
		uint64 numNormals = ReadUInt64(file);
		lmf->Normals.resize(numNormals);
		Read(file, &lmf->Normals[0], numNormals * sizeof(float));
	}
	if ((flags & (uint32)LMFFlags::HAS_TEXCOORDS) == (uint32)LMFFlags::HAS_TEXCOORDS) {
		uint64 numTexCoords = ReadUInt64(file);
		lmf->TexCoords.resize(numTexCoords);
		Read(file, &lmf->TexCoords[0], numTexCoords * sizeof(float));
	}

	return true;
}

bool WriteLFM(FILE *file, LanternModelFile *lmf) {
	// Write the header
	WriteUInt32(file, CreateMagicNumber('L', 'M', 'F', '\0'));
	WriteByte(file, lmf->VerticesPerPrimitive);
	
	// Calculate the flags directly, rather than requiring the user to do it
	uint32 flags = 0;
	uint64 numNormals = lmf->Normals.size();
	uint64 numTexCoords = lmf->TexCoords.size();

	if (numNormals > 0) {
		flags |= (uint32)LMFFlags::HAS_NORMALS;
	}
	if (numTexCoords > 0) {
		flags |= (uint32)LMFFlags::HAS_TEXCOORDS;
	}
	Lantern::WriteUInt32(file, flags);

	// Write the main data
	WriteUInt64(file, lmf->Positions.size() / 3);
	Write(file, &lmf->Positions[0], lmf->Positions.size() * sizeof(float));
	WriteUInt64(file, lmf->Indices.size());
	Write(file, &lmf->Indices[0], lmf->Indices.size() * sizeof(uint32));

	// Write the optional data
	if (numNormals > 0) {
		WriteUInt64(file, numNormals);
		Write(file, &lmf->Normals[0], numNormals * sizeof(float));
	}
	if (numTexCoords > 0) {
		WriteUInt64(file, numTexCoords);
		Write(file, &lmf->TexCoords[0], numTexCoords * sizeof(float));
	}

	return true;
}


} // End of namespace Lantern
