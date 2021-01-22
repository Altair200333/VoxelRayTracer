#pragma once

#include "Component.h"
#include "GuiDrawable.h"
#include "Vector3.h"

class Camera final: public Component, public GuiDrawable
{
public:
	float yaw = -90.0f;
	float pitch = 0.0f;

	Vector3 front;
	Vector3 right;
	Vector3 up;
	
	float fov;

	explicit Camera(Vector3 _front = {0, 1, 0}, Vector3 _right = {1, 0, 0}, Vector3 _up = {0, 0, 1}, float _fov = 60);

	[[nodiscard]] Vector3 getRayDirectionAtPixel(float x, float y, size_t width, size_t height) const;

	void updateCameraVectors();

	void onGuiDraw() override;
};
