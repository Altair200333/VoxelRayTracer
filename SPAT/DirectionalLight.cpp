#include "DirectionalLight.h"
#include "glm/ext/scalar_constants.hpp"
#include "ImGui/imgui.h"

Vector3 DirectionalLight::multiplyColors(const Vector3& otherColor) const
{
	return {otherColor.x * color.x, otherColor.y * color.y, otherColor.z * color.z};
}

void DirectionalLight::onGuiDraw()
{
	ImGui::Text("Affects the sharpness of shadows");
	ImGui::DragFloat("light area", &area, 1, 0, 100);
	ImGui::Text("Orientation");
	ImGui::DragFloat("Azimuth", &azimuth, 1, 0, 360);
	ImGui::DragFloat("Horizon", &horizon, 1, 0, 180);
	ImGui::DragFloat("Strength", &strength, 0.01f, 0, 1);
	ImGui::ColorEdit3("Color", reinterpret_cast<float*>(&color));
}

Vector3 DirectionalLight::getDirection() const
{
	const float x = sin(horizon * glm::pi<float>() / 180) * cos(azimuth * glm::pi<float>() / 180);
	const float y = sin(horizon * glm::pi<float>() / 180) * sin(azimuth * glm::pi<float>() / 180);
	const float z = cos(horizon * glm::pi<float>() / 180);

	return {x, y, z};
}
