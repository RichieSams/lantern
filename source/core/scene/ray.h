/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include "math/vector_types.h"
#include "math/align.h"


#define infinity std::numeric_limits<float>::infinity()
#define INVALID_GEOMETRY_ID ((uint)-1)
#define INVALID_PRIMATIVE_ID ((uint)-1)
#define INVALID_INSTANCE_ID ((uint)-1)


struct STRUCT_ALIGN(64) RTCRay {
public:
	/**
	 * Default constructor does nothing.
	 */
	__forceinline RTCRay() {
	}

	/**
	 * Constructs a ray from origin, direction, and ray segment.
	 *
	 * Note: Near has to be smaller than far.
	 */
	__forceinline RTCRay(const float3a& org, const float3a& dir,
	                     float tnear = 0.0f, float tfar = infinity,
	                     float time = 0.0f, uint mask = -1)
		: Origin(org),
		  Direction(dir),
		  TNear(tnear), TFar(tfar),
		  Time(time),
		  Mask(mask),
		  GeomID(INVALID_GEOMETRY_ID), PrimID(INVALID_PRIMATIVE_ID), InstID(INVALID_INSTANCE_ID) {
	}

public:
	float3a Origin;
	float3a Direction;
	float TNear;
	float TFar;
	float Time;
	uint Mask;
	float3a GeomNormal; // Not normalized
	float U; // Barycentric u coordinate of hit
	float V; // Barycentric v coordinate of hit
	int GeomID;
	int PrimID;
	int InstID;
};

typedef RTCRay Ray;

/*! Outputs ray to stream. */
inline std::ostream& operator<<(std::ostream& cout, const Ray& ray) {
	return std::cout << "{ " <<
		"origin = " << ray.Origin << ", direction = " << ray.Direction << ", near = " << ray.TNear << ", far = " << ray.TFar << ", time = " << ray.Time << ", " <<
		"instID = " << ray.InstID << ", geomID = " << ray.GeomID << ", primID = " << ray.PrimID << ", " << "u = " << ray.U << ", v = " << ray.V << ", geomNormal = " << ray.GeomNormal << " }";
}
