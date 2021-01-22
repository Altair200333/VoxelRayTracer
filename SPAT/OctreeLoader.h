#pragma once

class OctreeLoader
{
public:
	virtual ~OctreeLoader() = default;
	virtual void loadTo(const std::string& path, Octree<Vector3>& tree) = 0;
};