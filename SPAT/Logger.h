#pragma once
#include <deque>
#include <mutex>
#include <string>
#include "GuiSubscriber.h"

class Logger final: public GuiSubscriber
{	
	std::deque<std::string> logs;
	std::mutex mtx;

	void logMsg(const std::string& msg);
public:
	static Logger& instance();

	static void log(const std::string& msg);

	static std::string getLatestLogs();

	void onGuiDraw() override;
};
