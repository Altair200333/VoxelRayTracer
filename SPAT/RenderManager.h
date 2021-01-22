#pragma once
#include <memory>
#include <vector>

#include "OpenGLRenderer.h"
#include "SceneRenderer.h"

class RenderManager final: virtual public GuiDrawable
{
	std::vector<std::shared_ptr<SceneRenderer>> renderers;
	std::shared_ptr<Scene> scene;
	
	int item_current = 0;

public:
	explicit RenderManager(std::shared_ptr<Scene> _scene);

	std::shared_ptr<SceneRenderer> getCurrentRenderer();

	void updateScene();

	void onGuiDraw() override;
};
