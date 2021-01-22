#pragma once
#include "EventSubscriber.h"
#include "GuiDrawable.h"

//classes implementing this will automatically be draw when GuiEvent occurs
class GuiSubscriber: virtual public EventSubscriber, virtual public GuiDrawable
{
};