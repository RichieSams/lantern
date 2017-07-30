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
		: bsdf(nullptr),
		  medium(nullptr) {
	}
	explicit Material(BSDF *bsdf, Medium *medium = nullptr)
		: bsdf(bsdf),
		  medium(medium) {
	}

	BSDF *bsdf;
	Medium *medium;
};

} // End of namespace Lantern
