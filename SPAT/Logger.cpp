#include "Logger.h"
#include <sstream>
#include "ImGui/imgui.h"

void Logger::logMsg(const std::string& msg)
{
	mtx.lock();

	if (logs.size() > 20)
		logs.pop_front();
	logs.push_back(msg);
	
	if (stream)
		stream << msg << "\n";
	mtx.unlock();
}

Logger& Logger::instance()
{
	static Logger instance{};
	return instance;
}

void Logger::log(const std::string& msg)
{
	instance().logMsg(msg);
}

std::string Logger::getLatestLogs()
{
	std::stringstream out;
	auto& inst = instance();

	if (inst.logs.empty())
		return "";

	for (auto& msg : inst.logs)
	{
		out << msg << "\n";
	}
	return out.str();
}

void Logger::onGuiDraw()
{
	ImGui::Begin("Logs");
	ImGui::TextColored(ImVec4(0.9, 0.2, 0.9, 1), "Logs:\n%s", getLatestLogs().c_str());
	ImGui::End();
}
