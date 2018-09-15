/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "camera/pinhole_camera.h"


namespace Lantern {

PinholeCamera::PinholeCamera()
		: FrameBufferWidth(0),
		  FrameBufferHeight(0),
		  m_filter(ReconstructionFilter::Type::Tent),
		  m_phi((float)M_PI_2),
		  m_theta(0.0f),
		  m_radius(10.0f),
		  m_up(1.0f),
		  m_target(0.0f, 0.0f, 0.0f),
		  m_tanFovXDiv2(0.5773503f /* tan(60 degrees / 2) */),
		  m_tanFovYDiv2(tanf(0.5235f /* 60 degrees / 2 */ * 720 / 1280)) {
	UpdateOrigin();
	UpdateCartesianCoordSystem();
}

PinholeCamera::PinholeCamera(float phi, float theta, float radius, uint clientWidth, uint clientHeight, float3 target, float fov, ReconstructionFilter::Type filterType)
		: FrameBufferWidth(clientWidth),
		  FrameBufferHeight(clientHeight),
		  m_filter(filterType),
		  m_phi(phi),
		  m_theta(theta),
		  m_radius(radius),
		  m_up(1.0f),
		  m_target(target),
		  m_tanFovXDiv2(tanf(fov * 0.5f)),
		  m_tanFovYDiv2(tanf(fov * 0.5f * clientHeight / clientWidth)) {
	UpdateOrigin();
	UpdateCartesianCoordSystem();
}

void PinholeCamera::Rotate(float dPhi, float dTheta) {
	if (m_up > 0.0f) {
		m_theta += dTheta;
	} else {
		m_theta -= dTheta;
	}

	m_phi += dPhi;

	// Keep phi within -2PI to +2PI for easy 'up' comparison
	if (m_phi > 2.0 * (float)M_PI) {
		m_phi -= 2.0 * (float)M_PI;
	} else if (m_phi < -2.0 * (float)M_PI) {
		m_phi += 2.0 * (float)M_PI;
	}

	// If phi is between 0 to PI or -PI to -2PI, make 'up' be positive Y, other wise make it negative Y
	if ((m_phi > 0 && m_phi < M_PI) || (m_phi < -M_PI && m_phi > -2.0 * M_PI)) {
		m_up = 1.0f;
	} else {
		m_up = -1.0f;
	}

	UpdateOrigin();
	UpdateCartesianCoordSystem();
}

void PinholeCamera::Zoom(float distance) {
	m_radius -= distance;

	// Don't let the radius go negative
	// If it does, re-project our target down the look vector
	if (m_radius <= 0.0f) {
		m_radius = 30.0f;
		m_target -= m_zAxis * 30.0f;
	}

	UpdateOrigin();
}

void PinholeCamera::Pan(float dx, float dy) {
	m_target += (m_xAxis * dx) + (m_yAxis * dy);

	UpdateOrigin();
}

RTCRay PinholeCamera::CalculateRayFromPixel(uint x, uint y, UniformSampler *sampler) const {
	RTCRay ray;
	memset(&ray, 0, sizeof(ray));

	ray.org_x = m_origin.x;
	ray.org_y = m_origin.y;
	ray.org_z = m_origin.z;

	ray.tnear = 0.0f;
	ray.tfar = embree::inf;
	ray.mask = 0xFFFFFFFF;

	float u = m_filter.Sample(sampler->NextFloat());
	float v = m_filter.Sample(sampler->NextFloat());

	float3a viewVector((((x + 0.5f + u) / FrameBufferWidth) * 2.0f - 1.0f) * m_tanFovXDiv2,
		-(((y + 0.5f + v) / FrameBufferHeight) * 2.0f - 1.0f) * m_tanFovYDiv2,
		-1.0f);

	// Matrix multiply
	ray.dir_x = dot(viewVector, m_matrixMulXAxis);
	ray.dir_y = dot(viewVector, m_matrixMulYAxis);
	ray.dir_z = dot(viewVector, m_matrixMulZAxis);

	return ray;
}

} // End of namespace Lantern
