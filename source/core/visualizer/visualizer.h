/* Lantern - A path tracer
 *
 * Lantern is the legal property of Adrian Astley
 * Copyright Adrian Astley 2015 - 2016
 */

#pragma once

#include <atomic>
#include <chrono>

struct GLFWwindow;

namespace lantern {

class Scene;
struct PresentationBuffer;

class Visualizer {
public:
	Visualizer(PresentationBuffer *startingPresentationBuffer, std::atomic<uint64_t> *renderHostGenerationNumber, std::atomic<PresentationBuffer *> *swapPresentationBuffer);
	~Visualizer();

private:
	GLFWwindow *m_window;

	// Data from render host
	PresentationBuffer *m_currentPresentationBuffer;
	uint64_t m_presentationBufferGeneration;
	std::atomic<uint64_t> *m_renderHostGenerationNumber;
	std::atomic<PresentationBuffer *> *m_swapPresentationBuffer;

	// Render host GUI variables
	float m_renderTime[32];
	std::chrono::high_resolution_clock::time_point m_renderTimeStart;
	size_t m_renderTimeBin;

	// GUI variables
	float m_frameTime[32];
	size_t m_frameTimeBin;

	// Viewport variables
	float m_viewportZoom;

public:
	bool Init(int width, int height);
	void Run();
	void Shutdown();

private:
	bool RenderFrame();

	bool CreateSwapChain();
	bool CreateFrameBufferAndViews();
	bool RecreateSwapChain();
	void CleanupSwapChain();
};

} // End of namespace lantern
