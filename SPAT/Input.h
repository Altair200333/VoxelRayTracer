#pragma once
#include <GLFW/glfw3.h>
#include <map>
#include "OnUpdateSubscriber.h"
#include "KeyCodes.h"
class Input final: virtual public OnUpdateSubscriber
{
	std::map<KeyCode::Key, int> keyEvents;

	bool contains(KeyCode::Key key)
	{
		return keyEvents.find(key) != keyEvents.end();
	}
	
	static Input& instance()
	{
		static Input instance;
		return instance;
	}
	
	Input() = default;
public:
	static void setKeyState(int key, int action)
	{
		instance().keyEvents[key] = action;
	}
	
	static bool getKeyDown(KeyCode::Key key)
	{
		return instance().contains(key) && instance().keyEvents[key] == GLFW_PRESS;
	}
	static bool getKeyUp(KeyCode::Key key)
	{
		return instance().contains(key) && instance().keyEvents[key] == GLFW_RELEASE;
	}

	static bool getKeyPressed(KeyCode::Key key);

	void onUpdate() override
	{
		instance().keyEvents.clear();
	}
};

#include "Engine.h"
inline bool Input::getKeyPressed(KeyCode::Key key)
{
	return glfwGetKey(Engine::getWindow()->getGLFWwindow(), key) == GLFW_PRESS;
}
