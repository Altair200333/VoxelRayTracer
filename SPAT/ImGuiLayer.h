#pragma once
#include <GLFW/glfw3.h>

#include "RenderLayer.h"
#include "Viewport.h"
class ImGuiLayer final : public RenderLayer
{
	GLFWwindow* window;

	static void setParams(ImGuiIO& io);

	static void drawContents();

	void setDisplaySize(ImGuiIO& io) const;

	static void updateViewports(ImGuiIO& io);

public:

	explicit ImGuiLayer(GLFWwindow* _window);

	void onBegin() override;

	void onEnd() override;

	void onDraw() override;

	~ImGuiLayer();
};
