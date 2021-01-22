#include "Profiler.h"
#include "ImGui/imgui.h"

void Profiler::addFrameTime(float deltaTime)
{
	if (frameTimes.size() > frameSamples)
		frameTimes.pop_front();
	frameTimes.push_back(deltaTime);
}

float Profiler::getAverageFrametime()
{
	float totalTime = 0;
	for (auto time : frameTimes)
		totalTime += time;

	return totalTime / frameTimes.size();
}

Profiler& Profiler::instance()
{
	static Profiler instance;
	return instance;
}

void Profiler::startFrame()
{
	instance().start = std::chrono::high_resolution_clock::now();
}

void Profiler::endFrame()
{
	instance().end = std::chrono::high_resolution_clock::now();
	instance().addFrameTime(frameTime());
}

float Profiler::frameTime()
{
	return (instance().end - instance().start) / std::chrono::milliseconds(1);
}

void Profiler::onGuiDraw()
{
	ImGui::Begin("Profiler");
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "Frame time: %.2f", frameTime());
	ImGui::TextColored(ImVec4(0.2f, 0.8f, 0.4f, 1.0f), "fps:        %.2f", 1000.0f / frameTime());

	ImGui::Separator();

	ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.1f, 1.0f), "Average frame time: %.2f", getAverageFrametime());
	ImGui::TextColored(ImVec4(0.9f, 0.4f, 0.1f, 1.0f), "Average fps:        %.2f", 1000.0f / getAverageFrametime());

	ImGui::End();
}
