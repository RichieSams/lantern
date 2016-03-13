/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"
#include "math/vector_types.h"


namespace Lantern {

class UniformSampler {
public:
	UniformSampler(uint64 seed, uint64 sequence = 0)
		: m_state(seed),
		  m_sequence(sequence) {
		NextUInt();
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

		return (xorShifted >> rot) | (xorShifted << ((-rot) & 31));
	}

	float NextFloat() {
		uint32 temp = NextUInt();
		return std::ldexp(temp, -32);
	}

	float2 NextFloat2() {
		return float2(NextFloat(), NextFloat());
	}
};

} // End of namespace Lantern
