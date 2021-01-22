#include "RenderManager.h"
#include "RandomSampledRenderer.h"


RenderManager::RenderManager(std::shared_ptr<Scene> _scene): scene(_scene)
{
	renderers = {
		std::make_shared<OpenGLRenderer>(scene),
		std::make_shared<RandomSampledRenderer>(scene)
	};
}

std::shared_ptr<SceneRenderer> RenderManager::getCurrentRenderer()
{
	return renderers[item_current];
}

void RenderManager::updateScene()
{
	for (auto& renderer : renderers)
	{
		renderer->updateScene();
	}
}

void RenderManager::onGuiDraw()
{
	std::string names;
	std::for_each(renderers.begin(), renderers.end(), [&names]
              (const std::shared_ptr<SceneRenderer>& render)
	              {
		              names += render->name + '\0';
	              });
	ImGui::Combo("Renderer", &item_current, reinterpret_cast<const char*>(&names[0]), renderers.size());
}
