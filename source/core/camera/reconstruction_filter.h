/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once


namespace Lantern {

/**
 * A class for filtering the FrameBuffer
 *
 * The implementation is heavily based off of Benedikt Bitterli's Tungsten implementation
 * https://github.com/tunabrain/tungsten/blob/c6c68b4dbdae4ff4b6888a63b1e42965cb9fd001/src/core/cameras/ReconstructionFilter.hpp
 * which is licensed under the libpng/zlib license
 *
 * When I was trying to understand how the code works, I converted the implementation to python so I 
 * could have visualization: https://gist.github.com/RichieSams/ce2ca2a201227c1de543
 *
 * In turn, I created a question on the ComputerGraphics StackExchange, asking how this implementation compared to
 * the classic distribute and splat method (ie: https://github.com/mmp/pbrt-v3/blob/master/src/core/film.h#L114)
 * http://computergraphics.stackexchange.com/questions/2130/anti-aliasing-filtering-in-ray-tracing
 *
 * The community was able to elaborate on the method, pointing to an excellent paper that compared the two:
 * http://lgdv.cs.fau.de/publications/publication/Pub.2006.tech.IMMD.IMMD9.filter/
 * 
 * Using this information, I corrected the python code, then ported it back to c++ here
 */
class ReconstructionFilter {
public:
	enum class Type {
		Dirac,
		Box,
		Tent,
		Gaussian
	};

public:
	ReconstructionFilter();
	ReconstructionFilter(Type filterType);
	ReconstructionFilter(Type filterType, float width);

private:
	Type m_filterType;
	float m_width;
	float m_binSize;
	float m_invBinSize;
	
	#define NUM_BINS 32
	float m_cdf[NUM_BINS + 1];

public:
	float Sample(float x) const;
	float Evaluate(float x) const;

private:
	void PreCompute();
	float DefaultFilterWidth(Type filterType) const;
};

} // End of namespace Lantern
