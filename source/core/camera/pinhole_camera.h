/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"
#include "math/vector_types.h"
#include "math/uniform_sampler.h"

#include "camera/frame_buffer.h"
#include "camera/reconstruction_filter.h"

#define EMBREE_STATIC_LIB
#include "embree3/rtcore.h"


namespace Lantern {

class PinholeCamera {
public:
	PinholeCamera();
	PinholeCamera(float3 position, float3 target, float3 up, uint clientWidth, uint clientHeight, float fov = M_PI_2, ReconstructionFilter::Type filterType = ReconstructionFilter::Type::Tent);

public:
	uint FrameBufferWidth;
	uint FrameBufferHeight;

private:
	ReconstructionFilter m_filter;

	float3 m_position;

	// Ray Transform Data
	float m_tanFovXDiv2;
	float m_tanFovYDiv2;
	float3x3 m_transform;

public:
	/**
	 * Calculates the world-space ray from the camera origin to the specified pixel
	 *
	 * @param x         The x coordinate of the pixel
	 * @param y         The y coordinate of the pixel
	 */
	RTCRay CalculateRayFromPixel(uint x, uint y, UniformSampler *sampler) const;

private:
	void UpdateCartesianCoordSystem(float3 target, float3 up);
};

} // End of namespace Lantern
