#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "ResizeableTexture.h"
#include "TextureLoader.h"
#include "JPGTextureLoader.h"

class TextureLoadingManager final
{
	inline static std::map<std::string, std::shared_ptr<TextureLoader>> loaders =
	{
		{"jpg", std::make_shared<JPGTextureLoader>()}
	};
public:
	static std::optional<ResizeableTexture> load(const std::string& path)
	{
		const std::string extension = path.substr(path.find_last_of('.') + 1);
		if(loaders.count(extension)>0)
		{
			return loaders[extension]->load(path);
		}
		return {};
	}
};
