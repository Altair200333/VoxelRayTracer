#pragma once
#include <utility>
#include "GuiSubscriber.h"
#include "OnUpdateSubscriber.h"
#include "OpenGLRenderer.h"
#include "RandomSampledRenderer.h"
#include "RenderManager.h"
#include "Scene.h"
#include "Viewport.h"

class ViewportRenderer final: virtual public GuiSubscriber, virtual public OnUpdateSubscriber
{
	std::shared_ptr<Scene> scene;
	std::shared_ptr<Viewport> viewport = nullptr;

	RenderManager renderManager;
public:
	RenderManager& getRenderManager();

	explicit ViewportRenderer(std::shared_ptr<Scene> _scene);

	[[nodiscard]] auto getViewport() const
	{
		return viewport;
	}

	void init();

	void onGuiDraw() override;

	void onUpdate() override;
};
