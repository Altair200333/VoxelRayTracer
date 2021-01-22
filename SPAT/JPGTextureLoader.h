#pragma once
#include <optional>

#include "TextureLoader.h"

class JPGTextureLoader: public TextureLoader
{
	size_t textureInternalScale;
public:
	JPGTextureLoader(size_t textureScale = 1);

	std::optional<ResizeableTexture> load(const std::string& path) override;
};