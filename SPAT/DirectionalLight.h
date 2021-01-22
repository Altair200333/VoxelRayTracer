#pragma once
#include "Component.h"
#include "GuiDrawable.h"
#include "Vector3.h"

class DirectionalLight final: public Component, GuiDrawable
{
	float azimuth = 100;
	float horizon = 130;
public:
	float distanceFromWorld = 200;
	float strength = 1;
	Vector3 color = {1,1,1};

	[[nodiscard]] Vector3 multiplyColors(const Vector3& otherColor) const;

	void onGuiDraw() override;

	[[nodiscard]] Vector3 getDirection() const;
	float area = 5.2f;
};
