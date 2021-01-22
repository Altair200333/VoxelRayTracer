#pragma once
#include <GLFW/glfw3.h>

namespace KeyCode
{
	using Key = int;
	enum : Key
	{
		ESC = GLFW_KEY_ESCAPE,

		W = GLFW_KEY_W,
		A = GLFW_KEY_A,
		S = GLFW_KEY_S,
		D = GLFW_KEY_D,
		Q = GLFW_KEY_Q,
		E = GLFW_KEY_E,

		num1 = GLFW_KEY_1,
		num2 = GLFW_KEY_2,
		num3 = GLFW_KEY_3,
		num4 = GLFW_KEY_4,
		num5 = GLFW_KEY_5,
		num6 = GLFW_KEY_6,
		num7 = GLFW_KEY_7,
		num8 = GLFW_KEY_8,
		num9 = GLFW_KEY_9,
		num0 = GLFW_KEY_0,

		MMB = GLFW_MOUSE_BUTTON_MIDDLE,

		LShift = GLFW_KEY_LEFT_SHIFT,
		RShift = GLFW_KEY_RIGHT_SHIFT,

		LCtrl = GLFW_KEY_LEFT_CONTROL,
		RCtrl = GLFW_KEY_RIGHT_CONTROL,

		Space = GLFW_KEY_SPACE
	};
};
