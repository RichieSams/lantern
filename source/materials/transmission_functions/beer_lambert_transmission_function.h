/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include <cmath>


namespace Lantern {

class BeerLambertTransmissionFunction {
public:
	BeerLambertTransmissionFunction(float3 color, float atDistance)
		: m_absorptionCoefficient(-std::logf(color.x) / atDistance, -std::logf(color.y) / atDistance, -std::logf(color.z) / atDistance) {
		// Calculating the absorption coefficient this way is borrowed from Burley's 2015 Siggraph Course Notes "Extending the Disney BRDF to a BSDF with Integrated Subsurface Scattering"
		// It's much more intutive to specify a color and a distance, then back-calculate the coefficient
	}

private:
	const float3a m_absorptionCoefficient;

public:
	float3a Transmission(float distance) const {
		return float3a(std::expf(-m_absorptionCoefficient.x * distance), std::expf(-m_absorptionCoefficient.y * distance), std::expf(-m_absorptionCoefficient.z * distance));
	}
};

} // End of namespace Lantern
