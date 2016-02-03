/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "scene/camera.h"

#include <embree2/rtcore.h>


namespace Lantern {

Camera::Camera(uint clientWidth, uint clientHeight)
	: m_phi(M_PI_2),
	  m_theta(0.0f),
	  m_radius(10.0f),
	  m_up(1.0f),
	  m_target(0.0f, 0.0f, 0.0f),
	  m_clientWidth(clientWidth),
	  m_clientHeight(clientHeight),
	  m_tanFovXDiv2(0.5773503f /* tan(60 degrees / 2) */),
	  m_tanFovYDiv2(std::tanf(0.5235f /* 60 degrees / 2 */ * clientHeight / clientWidth)) {
}

Camera::Camera(float phi, float theta, float radius, uint clientWidth, uint clientHeight, float fov)
	: m_phi(phi),
	  m_theta(theta),
	  m_radius(radius),
	  m_up(1.0f),
	  m_target(0.0f, 0.0f, 0.0f),
	  m_clientWidth(clientWidth),
	  m_clientHeight(clientHeight),
	  m_tanFovXDiv2(std::tanf(fov * 0.5f)),
	  m_tanFovYDiv2(std::tanf(fov * 0.5f * clientHeight / clientWidth)) {
}

void Camera::Rotate(float dTheta, float dPhi) {
	if (m_up > 0.0f) {
		m_theta += dTheta;
	} else {
		m_theta -= dTheta;
	}

	m_phi += dPhi;

	// Keep phi within -2PI to +2PI for easy 'up' comparison
	if (m_phi > M_2_PI) {
		m_phi -= M_2_PI;
	} else if (m_phi < -M_2_PI) {
		m_phi += M_2_PI;
	}

	// If phi is between 0 to PI or -PI to -2PI, make 'up' be positive Y, other wise make it negative Y
	if ((m_phi > 0 && m_phi < M_PI) || (m_phi < -M_PI && m_phi > -M_2_PI)) {
		m_up = 1.0f;
	} else {
		m_up = -1.0f;
	}
}

void Camera::Zoom(float distance) {
	m_radius -= distance;

	// Don't let the radius go negative
	// If it does, re-project our target down the look vector
	if (m_radius <= 0.0f) {
		m_radius = 30.0f;
		float3 look = normalize(m_target - GetCameraPosition());

		m_target += look * 30.0f;
	}
}

void Camera::Pan(float dx, float dy) {
	float3 look = normalize(m_target - GetCameraPosition());

	float3 worldUp(0.0f, m_up, 0.0f);

	float3 right = normalize(cross(worldUp, look));
	float3 up = cross(look, right);

	m_target += (right * dx) + (up * dy);
}

RTCRay Camera::CalculateRayFromPixel(uint x, uint y) const {
	RTCRay ray;

	float3a origin = GetCameraPosition();
	ray.org = origin;

	ray.tnear = 0.0f;
	ray.tfar = embree::inf;
	ray.geomID = RTC_INVALID_GEOMETRY_ID;
	ray.primID = RTC_INVALID_GEOMETRY_ID;
	ray.instID = RTC_INVALID_GEOMETRY_ID;
	ray.mask = 0xFFFFFFFF;
	ray.time = 0.0f;

	float3a viewVector((((x + 0.5f) / m_clientWidth) * 2.0f - 1.0f) * m_tanFovXDiv2,
	                  -(((y + 0.5f) / m_clientHeight) * 2.0f - 1.0f) * m_tanFovYDiv2,
	                  -1.0f);

	float3a worldUp(0.0f, m_up, 0.0f);

	float3a zAxis = normalize(origin - float3a(m_target));
	float3a xAxis = normalize(cross(worldUp, zAxis));
	float3a yAxis =  cross(zAxis, xAxis);

	// Matrix multiply
	ray.dir = normalize(float3a(dot(viewVector, float3a(xAxis.x, yAxis.x, zAxis.x)),
	                            dot(viewVector, float3a(xAxis.y, yAxis.y, zAxis.y)),
	                            dot(viewVector, float3a(xAxis.z, yAxis.z, zAxis.z))));

	return ray;
}

} // End of namespace Lantern
