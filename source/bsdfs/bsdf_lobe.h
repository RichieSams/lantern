/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once


namespace Lantern {

namespace BSDFLobe {
enum Type {
	Null = 0,
	SpecularReflection = (1 << 0),
	SpecularTransmission = (1 << 1),
	Diffuse = (1 << 2),
	Specular = SpecularReflection | SpecularTransmission,
};
}

inline bool IsSpecular(BSDFLobe::Type lobe) { return (lobe & BSDFLobe::Specular) != 0;}

} // End of namespace Lantern
