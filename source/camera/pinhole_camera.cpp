/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#include "camera/pinhole_camera.h"

#include "scene/ray.h"


namespace Lantern {

PinholeCamera::PinholeCamera()
		: m_phi(M_PI_2),
		  m_theta(0.0f),
		  m_radius(10.0f),
		  m_up(1.0f),
		  m_target(0.0f, 0.0f, 0.0f),
		  m_tanFovXDiv2(0.5773503f /* tan(60 degrees / 2) */),
		  m_tanFovYDiv2(std::tanf(0.5235f /* 60 degrees / 2 */ * 720 / 1280)),
		  FrameBuffer(1280, 720),
		  m_filter(ReconstructionFilter::Type::Tent) {
	UpdateOrigin();
	UpdateCartesianCoordSystem();
	FrameBuffer.Reset();
}

PinholeCamera::PinholeCamera(float phi, float theta, float radius, float clientWidth, float clientHeight, float fov, ReconstructionFilter::Type filterType)
		: m_phi(phi),
		  m_theta(theta),
		  m_radius(radius),
		  m_up(1.0f),
		  m_target(0.0f, 0.0f, 0.0f),
		  m_tanFovXDiv2(std::tanf(fov * 0.5f)),
		  m_tanFovYDiv2(std::tanf(fov * 0.5f * clientHeight / clientWidth)),
		  FrameBuffer(clientWidth, clientHeight),
		  m_filter(filterType) {
	UpdateOrigin();
	UpdateCartesianCoordSystem();
	FrameBuffer.Reset();
}

void PinholeCamera::Rotate(float dPhi, float dTheta) {
	if (m_up > 0.0f) {
		m_theta += dTheta;
	} else {
		m_theta -= dTheta;
	}

	m_phi += dPhi;

	// Keep phi within -2PI to +2PI for easy 'up' comparison
	if (m_phi > 2.0 * M_PI) {
		m_phi -= 2.0 * M_PI;
	} else if (m_phi < -2.0 * M_PI) {
		m_phi += 2.0 * M_PI;
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
		m_target += m_zAxis * 30.0f;
	}

	UpdateOrigin();
}

void PinholeCamera::Pan(float dx, float dy) {
	m_target += (m_xAxis * dx) + (m_yAxis * dy);

	UpdateOrigin();
}

Ray PinholeCamera::CalculateRayFromPixel(uint x, uint y, UniformSampler *sampler) const {
	Ray ray;

	ray.Origin = m_origin;
	ray.TNear = 0.0f;
	ray.TFar = infinity;
	ray.GeomID = INVALID_GEOMETRY_ID;
	ray.PrimID = INVALID_PRIMATIVE_ID;
	ray.InstID = INVALID_INSTANCE_ID;
	ray.Mask = 0xFFFFFFFF;
	ray.Time = 0.0f;

	float u = m_filter.Sample(sampler->NextFloat());
	float v = m_filter.Sample(sampler->NextFloat());
	
	float3a viewVector((((x + 0.5f + u) / FrameBuffer.Width) * 2.0f - 1.0f) * m_tanFovXDiv2,
	                   -(((y + 0.5f + v) / FrameBuffer.Height) * 2.0f - 1.0f) * m_tanFovYDiv2,
	                   -1.0f);

	// Matrix multiply
	ray.Direction = normalize(float3a(dot(viewVector, m_matrixMulXAxis),
	                            dot(viewVector, m_matrixMulYAxis),
	                            dot(viewVector, m_matrixMulZAxis)));

	return ray;
}

} // End of namespace Lantern
