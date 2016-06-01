/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

namespace Lantern {

class BSDF;
class BeerLambertTransmissionFunction;

struct Material {
	Material()
		: BSDF(nullptr), 
		  TransmissionFunction(nullptr) {
	}
	Material(BSDF *bsdf, BeerLambertTransmissionFunction *transmissionFunction)
		: BSDF(bsdf),
		  TransmissionFunction(transmissionFunction) {
	}

	BSDF *BSDF;
	BeerLambertTransmissionFunction *TransmissionFunction;
};

} // End of namespace Lantern
