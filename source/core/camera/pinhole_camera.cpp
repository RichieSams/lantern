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
		  m_tanFovXDiv2(0.5773503f /* tan(60 degrees / 2) */),
		  m_tanFovYDiv2(tanf(0.5235f /* 60 degrees / 2 */ * 720 / 1280)) {
	UpdateCartesianCoordSystem(float3(0.0f), float3(0.0f, 1.0f, 0.0f));
}

PinholeCamera::PinholeCamera(float3 position, float3 target, float3 up, uint clientWidth, uint clientHeight, float fov, ReconstructionFilter::Type filterType)
		: FrameBufferWidth(clientWidth),
		  FrameBufferHeight(clientHeight),
		  m_filter(filterType),
		  m_position(position),
		  m_tanFovXDiv2(tanf(fov * 0.5f)),
		  m_tanFovYDiv2(tanf(fov * 0.5f * clientHeight / clientWidth)) {
	UpdateCartesianCoordSystem(target, up);
}

RTCRay PinholeCamera::CalculateRayFromPixel(uint x, uint y, UniformSampler *sampler) const {
	RTCRay ray;
	memset(&ray, 0, sizeof(ray));

	ray.org_x = m_position.x;
	ray.org_y = m_position.y;
	ray.org_z = m_position.z;

	ray.tnear = 0.0f;
	ray.tfar = std::numeric_limits<float>::infinity();
	ray.mask = 0xFFFFFFFF;

	float u = m_filter.Sample(sampler->NextFloat());
	float v = m_filter.Sample(sampler->NextFloat());

	float3 viewVector(
		(((x + 0.5f + u) / FrameBufferWidth) * 2.0f - 1.0f) * m_tanFovXDiv2,
		-(((y + 0.5f + v) / FrameBufferHeight) * 2.0f - 1.0f) * m_tanFovYDiv2,
		-1.0f);

	// Transform to world
	float3 world = m_transform * viewVector;
	ray.dir_x = world.x;
	ray.dir_y = world.y;
	ray.dir_z = world.z;

	return ray;
}

void PinholeCamera::UpdateCartesianCoordSystem(float3 target, float3 up) {
	float3 f = normalize(m_position - target);
	float3 r = normalize(cross(up, f));
	float3 u = normalize(cross(f, r));

	m_transform = float3x3{{r.x, u.x, f.x},
	                       {r.y, u.y, f.y},
	                       {r.z, u.z, f.z}};
}

} // End of namespace Lantern
