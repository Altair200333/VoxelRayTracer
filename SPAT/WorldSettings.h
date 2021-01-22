#pragma once
#include "BackgroundManager.h"
#include "GuiDrawable.h"
#include "Object.h"


class WorldSettings final: public GuiDrawable
{
public:
	BackgroundManager backgroundManager;
	Object directionalLight;

	explicit WorldSettings();

	void onGuiDraw() override;
};
