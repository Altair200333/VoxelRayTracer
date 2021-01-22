#pragma once
#include "Background.h"
#include "Vector3.h"
#include <glm/ext/scalar_constants.hpp>

#include "DirectionalLight.h"
#include <memory>

class SkyBackground final: public Background
{
	std::shared_ptr<DirectionalLight> light;
public:
	
	Vector3 skyColor = { 30.0f / 255, 30.0f / 255, 180.0f / 255 };
	Vector3 floorColor = { 20.0f / 255,20.0f / 255,30.0f / 255 };
	Vector3 horizonColor = { 209.0f / 255, 140.0f / 255, 30.0f / 255 };

	float horizonAngle = glm::pi<float>() * 1.05f / 2.0f;

	explicit SkyBackground(std::shared_ptr<DirectionalLight> _light);

	void onGuiDraw() override;

	[[nodiscard]] Vector3 getBackgroundColor(const Vector3& from, const Vector3& direction) const override;
};
