#pragma once

#include <string>
#include <vector>
#include "Octree.h"
#include "Vector3.h"


#include "OctreeLoader.h"

class PointCloudOctreeLoader final: public OctreeLoader
{
	static std::vector<std::pair<Vector3, Vector3>> readVoxelData(const std::string& path);
public:
	void loadTo(const std::string& path, Octree<Vector3>& tree) override;
};
