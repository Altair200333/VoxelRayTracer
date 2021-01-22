#pragma once
#include "TextureSaver.h"


class JPGTextureSaver final: public TextureSaver
{
public:
	void save(ResizeableTexture& texture, const std::string& path) override;
};
