#pragma once
#include <chrono>
#include "EventDispatcher.h"
#include "GuiSubscriber.h"

class Profiler final: public GuiSubscriber
{
	const size_t frameSamples = 100;

	std::chrono::steady_clock::time_point start;
	std::chrono::steady_clock::time_point end;

	std::list<float> frameTimes;
	void addFrameTime(float deltaTime);

	float getAverageFrametime();
public:
	static Profiler& instance();

	static void startFrame();

	static void endFrame();

	static float frameTime();

	void onGuiDraw() override;
};
