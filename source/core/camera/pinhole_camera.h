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
	float3 m_target;
	float3 m_up;

	// Ray Transform Data
	float m_tanFovXDiv2;
	float m_tanFovYDiv2;

	float3a m_matrixMulXAxis;
	float3a m_matrixMulYAxis;
	float3a m_matrixMulZAxis;

public:
	/**
	 * Calculates the world-space ray from the camera origin to the specified pixel
	 *
	 * @param x         The x coordinate of the pixel
	 * @param y         The y coordinate of the pixel
	 */
	RTCRay CalculateRayFromPixel(uint x, uint y, UniformSampler *sampler) const;

private:
	void UpdateCartesianCoordSystem() {
		float3a zAxis = normalize(m_position - m_target);
		float3a xAxis = normalize(cross(m_up, zAxis));
		float3a yAxis = cross(zAxis, xAxis);

		m_matrixMulXAxis = float3a(xAxis.x, yAxis.x, zAxis.x);
		m_matrixMulYAxis = float3a(xAxis.y, yAxis.y, zAxis.y);
		m_matrixMulZAxis = float3a(xAxis.z, yAxis.z, zAxis.z);
	}
};

} // End of namespace Lantern
