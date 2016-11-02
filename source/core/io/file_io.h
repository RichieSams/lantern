/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"

#include <cstdio>


namespace Lantern {

inline void Read(FILE *file, void *data, std::size_t length) {
	fread(data, sizeof(char), length, file);
}

inline byte ReadByte(FILE *file) {
	byte value;
	fread(&value, sizeof(byte), 1, file);

	return value;
}

inline uint32 ReadUInt32(FILE *file) {
	uint32 value;
	fread(&value, sizeof(uint32), 1, file);

	return value;
}

inline uint64 ReadUInt64(FILE *file) {
	uint64 value;
	fread(&value, sizeof(uint64), 1, file);

	return value;
}

inline float ReadFloat(FILE *file) {
	float value;
	fread(&value, sizeof(float), 1, file);

	return value;
}


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


inline uint32 CreateMagicNumber(char a0, char a1, char a2, char a3) {
	return (uint32)((a0) | ((a1) << 8) | ((a2) << 16) | ((a3) << 24));
}

inline bool VerifyMagicNumber(uint32 value, char a0, char a1, char a2, char a3) {
	uint32 number = (uint32)((a0) | ((a1) << 8) | ((a2) << 16) | ((a3) << 24));

	return number == value;
}

} // End of namespace Lantern
