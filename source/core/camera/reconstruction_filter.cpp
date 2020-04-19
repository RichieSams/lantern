/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "camera/reconstruction_filter.h"

#include "math/int_types.h"

#include <algorithm>
#include <cmath>


namespace Lantern {

ReconstructionFilter::ReconstructionFilter() 
		: m_filterType(Type::Tent),
		  m_width(DefaultFilterWidth(Type::Tent)) {
	PreCompute();	
}

ReconstructionFilter::ReconstructionFilter(Type filterType)
		: m_filterType(filterType),
		  m_width(DefaultFilterWidth(filterType)) {
	PreCompute();
}

ReconstructionFilter::ReconstructionFilter(Type filterType, float width)
		: m_filterType(filterType),
		  m_width(m_width) {
	PreCompute();
}

float ReconstructionFilter::Sample(float x) const {
	if (m_filterType == Type::Dirac) {
		return 0.0f;
	}
	if (m_filterType == Type::Box) {
		return x - m_width;
	}

	// All the pdf's/cdf's are computed on the range [0.0, 1.0]
	// However, we want to get an output in the range [-m_width, m_width]
	// So we split the random variable in half, expand it back to [0.0, 1.0],
	// and negate the result at the end if it's 'negative'
	//
	// 0.0              0.5              1.0
	// |----------------|----------------|
	//      negative         positive
	//
	//
	// 0.0              0.5
	// |----------------|
	//
	// x * 2.0
	//
	// 0.0                               1.0
	// |- - - - - - - - - - - - - - - - -|
	// 
	// 
	//                  0.5              1.0
	//                  |----------------|
	//
	// (x - 0.5) * 2.0
	//
	// 0.0                               1.0
	// |- - - - - - - - - - - - - - - - -|
	bool negative = x < 0.5f;
	x = negative ? x * 2.0f : (x - 0.5f) * 2.0f;

	// Find the last index of the cdf whose value is smaller
	// than our x value. This is the Inverse Tranform Method
	// For more information see here:
	// https://en.wikipedia.org/wiki/Inverse_transform_sampling
	// http://web.ics.purdue.edu/~hwan/IE680/Lectures/Chap08Slides.pdf Pages 8-6 and 8-7 in particular
	uint index = NUM_BINS;
	for (uint i = 1; i < NUM_BINS + 1; ++i) {
		if (x < m_cdf[i]) {
			index = i - 1;
			break;
		}
	}

	float pdf = m_cdf[index + 1] - m_cdf[index];
	float u = m_binSize * (index + ((x - m_cdf[index]) / pdf));

	return negative ? -u : u;
}

float ReconstructionFilter::Evaluate(float x) const {
	switch(m_filterType) {
	case Type::Box: 
		return (x >= -m_width && x <= m_width) ? 1.0f : 0.0f;
	case Type::Tent: 
		return m_width - std::abs(x);
	case Type::Gaussian: 
		{
			float alpha = 2.0f;
			return std::expf(-alpha * x * x) - std::expf(-alpha * m_width * m_width);
		}
	case Type::Dirac:
	default: 
		return 0.0f;
	}
}

void ReconstructionFilter::PreCompute() {
	// Dirac has an infinite pdf at 0.0f, aka it has no width.
	// Therefore, we can't use the inverse transform method.
	// That said, Dirac is so simple, we don't need to.
	//
	// We *can* compute the pdf for a box filter, but it's so
	// simple, it's pointless to do the extra math
	if (m_filterType == Type::Dirac || m_filterType == Type::Box) {
		return;
	}

	m_binSize = m_width / (NUM_BINS - 1);
	m_invBinSize = (NUM_BINS - 1) / m_width;

	// Discritize the filter into a binned pdf
	float normalizationFactor = 0.0f;
	float pdf[NUM_BINS];
	for (uint i = 0; i < NUM_BINS - 1; ++i) {
		float temp = Evaluate(i * m_binSize);
		pdf[i] = temp;
		normalizationFactor += temp;
	}
	// Gaurantee the last bin of the pdf is 0.0f
	pdf[NUM_BINS - 1] = 0.0f;

	// Normalize the pdf. AKA, we want the area under the curve to equal 1.0f
	for (uint i = 0; i < NUM_BINS - 1; ++i) {
		pdf[i] /= normalizationFactor;
	}

	// Create the cdf from the pdf
	m_cdf[0] = 0.0f;
	for (uint i = 1; i < NUM_BINS; ++i) {
		m_cdf[i] = m_cdf[i - 1] + pdf[i - 1];
	}
	m_cdf[NUM_BINS] = 1.0f;
}

float ReconstructionFilter::DefaultFilterWidth(Type filterType) const {
	switch(filterType) {
	case Type::Box:
		return 0.5f;
	case Type::Tent:
		return 1.0f;
	case Type::Gaussian:
		return 2.0f;
	case Type::Dirac: 
	default:
		return 0.0f;
	}
}
} // End of namespace Lantern
