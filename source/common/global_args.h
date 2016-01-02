/* Lantern - A path tracer
*
* Lantern is the legal property of Adrian Astley
* Copyright Adrian Astley 2015 - 2016
*/

#pragma once

#include <atomic>


namespace Lantern {
class AtomicFrameBuffer;

struct GlobalArgs {
	AtomicFrameBuffer *FrameBuffer;
	std::atomic_bool RenderChanged;

};

} // End of namespace Lantern
