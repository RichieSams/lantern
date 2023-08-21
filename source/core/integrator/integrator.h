/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

struct Scene;
struct FrameData;

extern "C" {
void RenderOneFrame(Scene *scene, FrameData *frameData, unsigned int frameNumber);
} // extern C
