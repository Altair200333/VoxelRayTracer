#pragma once
#include <string>
#include "ResizeableTexture.h"

class TextureLoader
{
public:
	virtual ~TextureLoader() = default;
	virtual std::optional<ResizeableTexture> load(const std::string& path) = 0;
};