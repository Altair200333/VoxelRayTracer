#pragma once
#include <map>
#include <memory>
#include <optional>
#include <string>

#include "FileUtils.h"
#include "OctreeLoader.h"
#include "PointCloudOctreeLoader.h"

class OctreeLoaderManager final
{
	//<file extension, associated loader> map
	inline static std::map<std::string, std::shared_ptr<OctreeLoader>> loaders = 
	{
		{"ply", std::make_shared<PointCloudOctreeLoader>()}
	};
public:
	static std::optional<std::shared_ptr<OctreeLoader>> getAssociatedLoader(const std::string& path)
	{
		const std::string extension = FileUtils::fileExtension(path);
		
		if(loaders.count(extension) > 0)
		{
			return { loaders[extension] };
		}
		return {};
	}
};
