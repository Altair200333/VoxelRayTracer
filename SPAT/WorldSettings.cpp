#include "WorldSettings.h"
#include "SkyBackground.h"
#include "ImageBackground.h"

WorldSettings::WorldSettings()
{
	directionalLight.addComponent(std::make_shared<DirectionalLight>());

	backgroundManager.addBackground(
		"sky", std::make_shared<SkyBackground>(directionalLight.getComponent<DirectionalLight>().value()));
	backgroundManager.addBackground("image", std::make_shared<ImageBackground>());
}

void WorldSettings::onGuiDraw()
{
	if (ImGui::TreeNode("Background"))
	{
		backgroundManager.onGuiDraw();
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("DirectionalLight"))
	{
		directionalLight.getComponent<DirectionalLight>().value()->onGuiDraw();
		ImGui::TreePop();
	}
}
