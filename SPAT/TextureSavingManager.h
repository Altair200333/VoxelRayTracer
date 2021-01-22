#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>
#include "JPGTextureSaver.h"
#include "ResizeableTexture.h"
#include "TextureSaver.h"


class TextureSavingManager final
{
	inline static std::map<std::string, std::shared_ptr<TextureSaver>> loaders =
	{
		{"jpg", std::make_shared<JPGTextureSaver>()}
	};
public:
	static bool save(ResizeableTexture& texture, const std::string& path)
	{
		const std::string extension = path.substr(path.find_last_of('.') + 1);
		if (loaders.count(extension) > 0)
		{
			loaders[extension]->save(texture, path);
			return true;
		}
		return false;
	}
};
