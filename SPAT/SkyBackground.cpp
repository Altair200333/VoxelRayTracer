#include "SkyBackground.h"
#include "ImGui/imgui.h"
#include <utility>

SkyBackground::SkyBackground(std::shared_ptr<DirectionalLight> _light): light(std::move(_light))
{
}

void SkyBackground::onGuiDraw()
{
	ImGui::DragFloat("Horizon angle", &horizonAngle, 0.01f, 0, glm::pi<float>());

	ImGui::ColorEdit3("Sky color", reinterpret_cast<float*>(&skyColor));
	ImGui::ColorEdit3("Floor color", reinterpret_cast<float*>(&floorColor));
	ImGui::ColorEdit3("Horizon color", reinterpret_cast<float*>(&horizonColor));
}

Vector3 SkyBackground::getBackgroundColor(const Vector3& from, const Vector3& direction) const
{
	const float horizon = acos(direction.z);
	const float azimuth = atan(direction.y / direction.x);

	if (horizon <= horizonAngle)
	{
		const float ratio = pow(cos(horizon * glm::pi<float>() / (2 * horizonAngle)), 0.8);
		const float dot = direction.dot(light->getDirection());

		Vector3 topColor = skyColor;

		if (dot < 0)
		{
			const Vector3 lightColor = light->color;
			const float lightRatio = exp(-(dot + 1) * (dot + 1) * 800) * light->strength;

			topColor = lightColor * lightRatio + skyColor * (1 - lightRatio);
		}
		return topColor * ratio + horizonColor * (1 - ratio);
	}
	else
	{
		const float ratio = pow(cos(horizon - horizonAngle), 30);
		return horizonColor * ratio + floorColor * (1 - ratio);
	}
}
