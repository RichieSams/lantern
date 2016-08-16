/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"

#include <cstdio>


namespace Lantern {

inline void Write(FILE *file, void *data, std::size_t length) {
	fwrite(data, sizeof(char), length, file);
}

inline void WriteMagicNumber(FILE *file, char a0, char a1, char a2, char a3) {
	uint32 number = (uint32)((a0) | ((a1) << 8) | ((a2) << 16) | ((a3) << 24));
	fwrite(&number, sizeof(uint32), 1, file);
}

inline void WriteByte(FILE *file, byte value) {
	fwrite(&value, sizeof(byte), 1, file);
}

inline void WriteUInt32(FILE *file, uint32 value) {
	fwrite(&value, sizeof(uint32), 1, file);
}

inline void WriteUInt64(FILE *file, uint64 value) {
	fwrite(&value, sizeof(uint64), 1, file);
}

inline void WriteFloat(FILE *file, float value) {
	fwrite(&value, sizeof(float), 1, file);
}

} // End of namespace Lantern
