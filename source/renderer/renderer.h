/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include <embree2/rtcore.h>


namespace Lantern {
struct GlobalArgs;

class Renderer {
public:
	Renderer(GlobalArgs *globalArgs);
	~Renderer();

private:
	GlobalArgs *m_globalArgs;

	RTCDevice m_device;
	RTCScene m_scene;

public:
	void SetScene();
	void Start();

};

} // End of namespace Lantern
