#pragma once
#include <deque>
#include <mutex>
#include <string>
#include "GuiSubscriber.h"
#include <fstream>

class Logger final: public GuiSubscriber
{
	inline static std::string loggerFileName = "log.txt";
	std::ofstream stream;

	std::deque<std::string> logs;
	std::mutex mtx;

	void logMsg(const std::string& msg);
	Logger() : stream(std::ofstream(loggerFileName, std::ios::app))
	{
	}
public:
	static Logger& instance();

	static void log(const std::string& msg);

	static std::string getLatestLogs();

	void onGuiDraw() override;
};
