/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#include "primitives/uniform_infinite_sphere.h"

#include "math/sampling.h"

#include <limits>

namespace Lantern {

void UniformInfiniteSphere::Initialize(float3 emissiveColor, float radiance) {
	Primitive::Initialize(emissiveColor * radiance, nullptr, nullptr, 0, -1, false, false);
}

float3 UniformInfiniteSphere::SampleDirectLighting(UniformSampler *sampler, SurfaceInteraction &interaction, float3 *direction, float *distance, float *pdf) const {
    *direction = UniformSampleHemisphere(interaction.Normal, sampler);
	*distance = std::numeric_limits<float>::infinity();
    *pdf = UniformHemispherePdf();

    return m_emission;
}

} // End of namespace Lantern
