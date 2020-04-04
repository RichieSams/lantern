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
	Reflection = 1 << 0,
	Transmission = 1 << 1,
	Diffuse = 1 << 2,
	Glossy = 1 << 3,
	Specular = 1 << 4,
	All = Specular | Glossy | Diffuse | Transmission | Reflection,
};
}

inline bool IsSpecular(BSDFLobe::Type lobe) { return (lobe & BSDFLobe::Specular) != 0;}

} // End of namespace Lantern
