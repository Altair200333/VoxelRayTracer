#include "ViewportRenderer.h"


#include "CameraController.h"
#include "Input.h"

RenderManager& ViewportRenderer::getRenderManager()
{
	return renderManager;
}

ViewportRenderer::ViewportRenderer(std::shared_ptr<Scene> _scene): scene(std::move(_scene)), renderManager(scene)
{
}

void ViewportRenderer::init()
{
	viewport = std::make_shared<Viewport>();
	viewport->renderer = std::make_shared<OpenGLRenderer>(scene);
}

void ViewportRenderer::onGuiDraw()
{
	viewport->onGuiDraw();
	ImGui::Begin("Level");

	renderManager.onGuiDraw();

	viewport->renderer = renderManager.getCurrentRenderer();

	if (ImGui::CollapsingHeader("Renderer settings"))
	{
		if (viewport->renderer)
			viewport->renderer->onGuiDraw();
	}

	scene->onGuiDraw();

	ImGui::End();
}

void ViewportRenderer::onUpdate()
{
	if (Input::getKeyDown(KeyCode::Space))
	{
		Logger::log("Render");

		viewport->getTexture().reset();
		viewport->getLock() = true;
		scene->getCamera().getComponent<CameraController>().value()->enabled = false;
		std::thread([&]()
		{
			viewport->renderer->fullscaleRender(viewport->getTexture());
			viewport->getLock() = false;
			scene->getCamera().getComponent<CameraController>().value()->enabled = true;
		}).detach();
	}
}
