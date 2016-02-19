/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/typedefs.h"
#include "math/vector_types.h"


namespace Lantern {

class UniformSampler {
public:
	UniformSampler(uint64 seed, uint64 sequence = 0)
		: m_state(seed),
		  m_sequence(sequence) {
	}

private:
	uint64 m_state;
	const uint64 m_sequence;

public:
	// PCG psuedo-random number generator
	// http://www.pcg-random.org/
	uint32 NextUInt() {
		uint64 oldState = m_state;
		
		m_state = oldState * 6364136223846793005ULL + (m_sequence | 1);
		uint32 xorShifted = ((oldState >> 18u) ^ oldState) >> 27u;
		uint32 rot = oldState >> 59u;

		return (xorShifted >> rot) | (xorShifted << (uint32(-int32(rot)) & 31));
	}

	// Implementation of this paper: http://www.doornik.com/research/randomdouble.pdf
	#define INVERSE_MAX_UINT32 2.32830643653869628906e-010 /* 1 / (2^-32) */
	#define UINT_TO_UNIFORM_FLOAT(input) ((int)(input) * INVERSE_MAX_UINT32 + (0.5 + INVERSE_MAX_UINT32 / 2))

	float NextFloat() {
		return UINT_TO_UNIFORM_FLOAT(NextUInt());
	}

	float2 NextFloat2() {
		return float2(NextFloat(), NextFloat());
	}
};

} // End of namespace Lantern
