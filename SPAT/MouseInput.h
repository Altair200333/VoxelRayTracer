#pragma once
#include <GLFW/glfw3.h>

#include "Engine.h"

class MouseInput final : virtual public OnUpdateSubscriber
{
	double mouseX = 0;
	double mouseY = 0;
	bool firstMouse = true;

	double deltaX = 0;
	double deltaY = 0;

	static MouseInput& instance()
	{
		static MouseInput instance;
		return instance;
	}
	MouseInput() = default;
public:
	static double dx()
	{
		return instance().deltaX;
	}
	static double dy()
	{
		return instance().deltaY;
	}
	static bool getMouseButton(int keyCode)
	{
		return glfwGetMouseButton(Engine::getWindow()->getGLFWwindow(), keyCode);
	}
	static void moveMouse(double xPos, double yPos)
	{
		auto& inst = instance();
		if (inst.firstMouse)
		{
			inst.mouseX = xPos;
			inst.mouseY = yPos;
			inst.firstMouse = false;
		}

		inst.deltaX = xPos - inst.mouseX;
		inst.deltaY = inst.mouseY - yPos;

		inst.mouseX = xPos;
		inst.mouseY = yPos;
	}
	
	void onUpdate() override
	{
		instance().deltaY = instance().deltaX = 0;
	}
};
