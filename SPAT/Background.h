#pragma once
#include "GuiDrawable.h"
#include "Vector3.h"

class Background: public GuiDrawable
{
public:
	virtual ~Background() = default;
	[[nodiscard]] virtual Vector3 getBackgroundColor(const Vector3& from, const Vector3& direction) const = 0;
};
