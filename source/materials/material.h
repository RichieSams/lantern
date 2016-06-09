/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

namespace Lantern {

class BSDF;
class Medium;

struct Material {
	Material()
		: BSDF(nullptr), 
		  Medium(nullptr) {
	}
	explicit Material(BSDF *bsdf, Medium *medium = nullptr)
		: BSDF(bsdf),
		  Medium(medium) {
	}

	BSDF *BSDF;
	Medium *Medium;
};

} // End of namespace Lantern
