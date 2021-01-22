#pragma once
#include "Background.h"

#include "TextureLoadingManager.h"


class ImageBackground final: public Background
{
public:
	std::optional<ResizeableTexture> backgroundImage;

	ImageBackground();

	[[nodiscard]] Vector3 getBackgroundColor(const Vector3& from, const Vector3& direction) const override;

	void onGuiDraw() override;
};
