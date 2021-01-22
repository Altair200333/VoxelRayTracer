#pragma once
#include <string>

#include "ResizeableTexture.h"

class TextureSaver
{
public:
	virtual ~TextureSaver() = default;
	virtual void save(ResizeableTexture& texture, const std::string& path) = 0;
};