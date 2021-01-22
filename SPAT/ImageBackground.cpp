#include "ImageBackground.h"
#include "glm/ext/scalar_constants.hpp"
#include "Logger.h"
#include "ImGui/imgui.h"
#include "FileUtils.h"

ImageBackground::ImageBackground()
{
	backgroundImage = JPGTextureLoader().load("env/env2.jpg");
}

Vector3 ImageBackground::getBackgroundColor(const Vector3& from, const Vector3& direction) const
{
	if (!backgroundImage)
		return {};

	auto& image = backgroundImage.value();
	const float theta = atan2(direction.y, direction.x) * 180 / glm::pi<float>() + 180;
	const float alpha = atan2(direction.z, sqrt(direction.x * direction.x + direction.y * direction.y)) * 180 / glm::pi<
		float>() + 90;

	const float x = image.width * theta / 360;
	const float y = image.height * alpha / 180;

	return image.get(image.width - x - 1, image.height - 1 - y) * (1 / 255.0f);
}

void ImageBackground::onGuiDraw()
{
	if (ImGui::Button("Select background image"))
	{
		const auto maybePath = FileUtils::openFileDialog();
		if (maybePath)
		{
			const auto maybeBackground = TextureLoadingManager::load(maybePath.value());
			if (maybeBackground)
				backgroundImage = maybeBackground;
			else
				Logger::log("Could not load image");
		}
		else
		{
			Logger::log("Background image not selected");
		}
	}
}
