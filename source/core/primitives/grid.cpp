/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "primitives/grid.h"

#include "integrator/surface_interaction.h"

#include "math/vector_math.h"
#include "math/sampling.h"


namespace Lantern {

bool Grid::Initialize(RTCDevice device, RTCScene scene, float width, float depth, float3 emissiveColor, float radiantPower, float4x4 transform, BSDF *bsdf, Medium *medium) {
	m_origin = (transform * float4(-width * 0.5f, 0.0f, -depth * 0.5f, 1.0f)).xyz();
	m_x = (transform * float4(1.0f, 0.0f, 0.0f, 1.0f)).xyz();
	m_z = (transform * float4(0.0f, 0.0f, 1.0f, 1.0f)).xyz();
	
	RTCGeometry geometry = rtcNewGeometry(device, RTC_GEOMETRY_TYPE_GRID);
	rtcSetGeometryBuildQuality(geometry, RTC_BUILD_QUALITY_HIGH);
	rtcSetGeometryTimeStepCount(geometry, 1);

	float3a *vertices = (float3a *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_VERTEX, 0, RTC_FORMAT_FLOAT3, sizeof(float3a), 4);
	vertices[0] = transform * float3a(-width * 0.5f, 0.0f, -depth * 0.5f, 1.0f);
	vertices[1] = transform * float3a( width * 0.5f, 0.0f, -depth * 0.5f, 1.0f);
	vertices[2] = transform * float3a(-width * 0.5f, 0.0f,  depth * 0.5f, 1.0f);
	vertices[3] = transform * float3a( width * 0.5f, 0.0f,  depth * 0.5f, 1.0f);

	RTCGrid *grid = (RTCGrid *)rtcSetNewGeometryBuffer(geometry, RTC_BUFFER_TYPE_GRID, 0, RTC_FORMAT_GRID, sizeof(RTCGrid), 1);
	memset(grid, 0, sizeof(grid));
	grid->startVertexID = 0;
	grid->stride = 2;
	grid->width = 2;
	grid->height = 2;

	rtcCommitGeometry(geometry);
	uint geometryId = rtcAttachGeometry(scene, geometry);
	rtcReleaseGeometry(geometry);

	const float surfaceArea = length(m_x) * length(m_z);
	Primitive::Initialize(emissiveColor * radiantPower * (float)M_1_PI / surfaceArea, bsdf, medium, surfaceArea, geometryId, false, false);

	return true;
}

float3 Grid::SampleDirectLighting(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *direction, float *distance, float *pdf) const {
	float2 rand = sampler->NextFloat2();

	float3 position = m_origin + rand.x * m_x + rand.y * m_z;
	float3 absoluteDirection = position - interaction.Position;

	*distance = length(absoluteDirection);
	*direction = normalize(absoluteDirection);
	*pdf = 1.0f / (m_surfaceArea);

	return m_emission;
}

} // End of namespace Lantern
