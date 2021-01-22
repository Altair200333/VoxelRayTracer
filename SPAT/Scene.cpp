#include "Scene.h"
#include "Transform.h"
#include "CameraController.h"
Scene::Scene()
{
	camera.addComponent(std::make_shared<Transform>(Transform({0, -50, 10})));
	camera.addComponent(std::make_shared<Camera>());
	camera.addComponent(std::make_shared<CameraController>());
}

void Scene::onGuiDraw()
{
	if (ImGui::CollapsingHeader("Camera settings"))
	{
		camera.getComponent<CameraController>().value()->onGuiDraw();
		camera.getComponent<Camera>().value()->onGuiDraw();
	}
	if (ImGui::CollapsingHeader("Scene"))
	{
		world.onGuiDraw();
	}
}
