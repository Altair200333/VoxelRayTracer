#pragma once

class GuiDrawable
{
public:
	virtual ~GuiDrawable() = default;
	virtual void onGuiDraw() = 0;
};