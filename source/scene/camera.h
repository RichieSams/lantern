/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "common/typedefs.h"

#include "math/vector_types.h"

#include "scene/ray.h"


namespace Lantern {

class Camera {
public:
	Camera(uint clientWidth, uint clientHeight);
	Camera(float phi, float theta, float radius, uint clientWidth, uint clientHeight, float fov = M_PI_2);

private:
	float m_phi;
	float m_theta;
	float m_radius;

	float m_up;

	float3 m_target;

	uint m_clientWidth;
	uint m_clientHeight;

	float m_tanFovXDiv2;
	float m_tanFovYDiv2;

public:
	/**
	 * Rotate the camera about a point in front of it (m_target). Theta rotates the camera
	 * side to side. Phi tilts the camera up and down.
	 * See this picture for details: https://upload.wikimedia.org/wikipedia/commons/d/dc/3D_Spherical_2.svg
	 *
	 * @param dTheta    The number of radians to rotate in the theta direction
	 * @param dPhi      The number of radians to rotate in the phi direction
	 */
	void Rotate(float dTheta, float dPhi);
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
	* Returns the position of the camera in Cartesian coordinates
	*
	* @return    The position of the camera
	*/
	float3 GetCameraPosition() const {
		float x = m_radius * std::cosf(m_phi); 
		float y = m_radius * std::sinf(m_phi) * std::sinf(m_theta);
		float z = m_radius * std::sinf(m_phi) * std::cosf(m_theta);

		return m_target + float3(x, y, z);
	}

	/**
	 * Calculates the world-space ray from the camera origin to the specified pixel
	 *
	 * @param x         The x coordinate of the pixel
	 * @param y         The y coordinate of the pixel
	 */
	RTCRay CalculateRayFromPixel(uint x, uint y) const;
};

} // End of namespace Lantern
