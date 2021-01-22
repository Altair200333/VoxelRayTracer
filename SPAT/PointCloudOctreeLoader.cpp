#include "PointCloudOctreeLoader.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iterator>

std::vector<std::pair<Vector3, Vector3>> PointCloudOctreeLoader::readVoxelData(const std::string& path)
{
	std::ifstream file(path);
	std::string line;
	while (std::getline(file, line))
	{
		if (line == "end_header")
			break;
	}

	std::vector<std::pair<Vector3, Vector3>> voxels;
	while (std::getline(file, line))
	{
		std::vector<float> v;

		std::istringstream iss(line);

		std::copy(std::istream_iterator<float>(iss),
		          std::istream_iterator<float>(),
		          std::back_inserter(v));
		if (v.size() != 6)
			continue;
		voxels.push_back(std::make_pair<Vector3, Vector3>({v[0], v[1], v[2]}, {v[3], v[4], v[5]}));
	}
	return voxels;
}

void PointCloudOctreeLoader::loadTo(const std::string& path, Octree<Vector3>& tree)
{
	const float voxSize = 1;
	std::vector<std::pair<Vector3, Vector3>> voxels = readVoxelData(path);

	Vector3 average;
	Vector3 maxes = Vector3(-std::numeric_limits<float>::max());
	Vector3 mins = Vector3(std::numeric_limits<float>::max());

	for (auto& [pos, color] : voxels)
	{
		average += pos;
		maxes.x = std::fmax(pos.x, maxes.x);
		maxes.y = std::fmax(pos.y, maxes.y);
		maxes.z = std::fmax(pos.z, maxes.z);

		mins.x = std::fmin(pos.x, mins.x);
		mins.y = std::fmin(pos.y, mins.y);
		mins.z = std::fmin(pos.z, mins.z);
	}
	average = average * (1.0f / voxels.size());

	const float treeSpan = std::max({
		std::fabs(maxes.x - mins.x), std::fabs(maxes.y - mins.y), std::fabs(maxes.z - mins.z)
	}) + 1;
	const float treeDepth = std::ceilf(log(treeSpan) / log(2)) + 1;

	tree.clear();
	tree.size = std::powf(2, treeDepth);
	tree.recalculateBounds();

	for (auto& [pos, color] : voxels)
	{
		tree.insertVoxelAtDepth(pos + Vector3(voxSize * 0.5f), color * (1.0f / 255.0f), treeDepth);
	}
}
