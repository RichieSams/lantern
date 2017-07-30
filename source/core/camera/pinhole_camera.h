/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/int_types.h"
#include "math/vector_types.h"
#include "math/uniform_sampler.h"

#include "scene/ray.h"

#include "camera/frame_buffer.h"
#include "camera/reconstruction_filter.h"


namespace Lantern {

class PinholeCamera {
public:
	PinholeCamera();
	PinholeCamera(float phi, float theta, float radius, uint clientWidth, uint clientHeight, float3 target = float3(0.0f), float fov = M_PI_2, ReconstructionFilter::Type filterType = ReconstructionFilter::Type::Tent);
	
public:
	// Frame Data
	FrameBuffer FrameBufferData;

private:
	ReconstructionFilter m_filter;

	// Spherical data
	float m_phi;
	float m_theta;
	float m_radius;

	float m_up;

	float3 m_target;

	// Cartesian Data
	float3 m_origin;

	float3 m_xAxis;
	float3 m_yAxis;
	float3 m_zAxis;

	// Ray Transform Data
	float m_tanFovXDiv2;
	float m_tanFovYDiv2;

	float3a m_matrixMulXAxis;
	float3a m_matrixMulYAxis;
	float3a m_matrixMulZAxis;

public:
	/**
	 * Rotate the camera about a point in front of it (m_target). 
	 * 
	 * Phi tilts the camera up and down. 
	 * Theta rotates the camera side to side. 
	 *
	 * @param dPhi      The number of radians to rotate in the phi direction
	 * @param dTheta    The number of radians to rotate in the theta direction
	 */
	void Rotate(float dPhi, float dTheta);
	/**
	 * Move the camera down the look vector, closer to m_target. If we overtake m_target,
	 * it is reprojected 30 units down the look vector
	 *
	 * TODO: Find a way to *not* hard-code the reprojection distance. Perhaps base it on the
	 *       scene size? 
	 *
	 * @param distance    The distance to zoom. Negative distance will move the camera away from the target, positive will move towards
	 */
	void Zoom(float distance);
	/**
	 * Moves the camera within its local X-Y plane
	 *
	 * @param dx    The amount to move the camera right or left
	 * @param dy    The amount to move the camera up or down
	 */
	void Pan(float dx, float dy);

	/**
	 * Calculates the world-space ray from the camera origin to the specified pixel
	 *
	 * @param x         The x coordinate of the pixel
	 * @param y         The y coordinate of the pixel
	 */
	Ray CalculateRayFromPixel(uint x, uint y, UniformSampler *sampler) const;

private:
	/**
	* Returns the position of the camera in Cartesian coordinates
	*
	* @return    The position of the camera
	*/
	void UpdateOrigin() {
		float x = m_radius * sinf(m_phi) * sinf(m_theta);
		float y = m_radius * cosf(m_phi);
		float z = m_radius * sinf(m_phi) * cosf(m_theta);

		m_origin = m_target + float3(x, y, z);
	}

	void UpdateCartesianCoordSystem() {
		m_zAxis = normalize(m_origin - m_target);
		if (m_zAxis.x == 0.0 && (m_zAxis.y == 1.0f || m_zAxis.y == -1.0f) && m_zAxis.z == 0.0f) {
			m_xAxis = normalize(cross(float3(0.0f, 0.0f, m_up), m_zAxis));
		} else {
			m_xAxis = normalize(cross(float3(0.0f, m_up, 0.0f), m_zAxis));
		}
		m_yAxis = cross(m_zAxis, m_xAxis);

		m_matrixMulXAxis = float3a(m_xAxis.x, m_yAxis.x, m_zAxis.x);
		m_matrixMulYAxis = float3a(m_xAxis.y, m_yAxis.y, m_zAxis.y);
		m_matrixMulZAxis = float3a(m_xAxis.z, m_yAxis.z, m_zAxis.z);
	}
};

} // End of namespace Lantern
