#include "JPGTextureLoader.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

JPGTextureLoader::JPGTextureLoader(size_t textureScale): textureInternalScale(textureScale)
{
}

std::optional<ResizeableTexture> JPGTextureLoader::load(const std::string& path)
{
	int width, height, components;
	uint8_t* data = stbi_load(path.c_str(), &width, &height, &components, 0);

	if (data == nullptr)
		return {};

	ResizeableTexture bitmap(data, width, height, components);

	stbi_image_free(data);

	return bitmap;
}
