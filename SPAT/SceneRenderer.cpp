#include "SceneRenderer.h"

#include "Logger.h"

const std::shared_ptr<Scene>& SceneRenderer::getScene() const
{
	return scene;
}

std::optional<SceneRenderer::RenderContext> SceneRenderer::getRenderContext() const
{
	auto& cameraObject = scene->camera;
	const auto maybeCam = cameraObject.getComponent<Camera>();
	const auto maybeTransform = cameraObject.getComponent<Transform>();
	const auto maybeLight = scene->world.directionalLight.getComponent<DirectionalLight>();

	if (!maybeCam || !maybeTransform || !maybeLight)
	{
		Logger::log("Transform and camera components are required for rendering");
		return {};
	}

	return {{maybeCam.value(), maybeTransform.value(), maybeLight.value()}};
}
