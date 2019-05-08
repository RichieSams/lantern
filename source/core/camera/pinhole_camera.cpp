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
		  m_position(0.0f, 0.0f, -10.0f),
		  m_target(0.0f, 0.0f, 0.0f),
		  m_up(1.0f),
		  m_tanFovXDiv2(0.5773503f /* tan(60 degrees / 2) */),
		  m_tanFovYDiv2(tanf(0.5235f /* 60 degrees / 2 */ * 720 / 1280)) {
	UpdateCartesianCoordSystem();
}

PinholeCamera::PinholeCamera(float3 position, float3 target, float3 up, uint clientWidth, uint clientHeight, float fov, ReconstructionFilter::Type filterType)
		: FrameBufferWidth(clientWidth),
		  FrameBufferHeight(clientHeight),
		  m_filter(filterType),
		  m_position(position),
		  m_target(target),
		  m_up(up),
		  m_tanFovXDiv2(tanf(fov * 0.5f)),
		  m_tanFovYDiv2(tanf(fov * 0.5f * clientHeight / clientWidth)) {
	UpdateCartesianCoordSystem();
}

RTCRay PinholeCamera::CalculateRayFromPixel(uint x, uint y, UniformSampler *sampler) const {
	RTCRay ray;
	memset(&ray, 0, sizeof(ray));

	ray.org_x = m_position.x;
	ray.org_y = m_position.y;
	ray.org_z = m_position.z;

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
