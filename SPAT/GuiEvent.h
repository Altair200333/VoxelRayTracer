#pragma once
#include "Event.h"
#include "GuiSubscriber.h"

class GuiEvent final : public Event<GuiSubscriber>
{
public:

	void handle(GuiSubscriber* var) const override
	{
		var->onGuiDraw();
	}
};
