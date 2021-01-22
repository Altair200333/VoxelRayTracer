#pragma once
#include <cstdint>
#include "Ray.h"
#include "Vector3.h"
#include "VectorMath.h"


template<typename T>
class Octree final
{
	//defines location of children nodes based on index
	inline static const Vector3 offsets[8] = {
		{-1, -1, -1},
		{-1, -1, 1},
		{-1, 1, -1},
		{-1, 1, 1},
		{1, -1, -1},
		{1, -1, 1},
		{1, 1, -1},
		{1, 1, 1}
	};
	static constexpr size_t maxChildren = 8;
public:
	size_t count = 0;
	Vector3 bounds[2];
	T value;
	
	//size of voxel cube
	float size = 2;
	Vector3 center;
	bool isLeaf = false;
	std::array<Octree*, maxChildren> nodes = { nullptr };

	void recalculateBounds()
	{
		bounds[0] = { center.x - size / 2,center.y - size / 2,center.z - size / 2 };
		bounds[1] = { center.x + size / 2,center.y + size / 2,center.z + size / 2 };
	}

	explicit Octree(const T& _val = T{}, Vector3 _center = { 0,0,0 }, float _size = 2, bool leaf = false) : isLeaf(leaf), center(_center), size(_size), value(_val)
	{
		recalculateBounds();
	}

	void insertVoxelAtDepth(const Vector3& position, T data, int targetDepth)
	{
		insertVoxelPosition(position, data, this, 0, targetDepth);
	}
	
	[[nodiscard]] bool isInVoxel(const Vector3& position) const
	{
		return VectorMath::isInBox(position, center, size * 0.5f);
	}
	
	void clear()
	{
		clearChildren(this);
	}
	
	~Octree()
	{
		clear();
	}
private:
	void clearChildren(Octree* node)
	{
		for(size_t i=0; i < maxChildren; ++i)
		{
			if(node->nodes[i] != nullptr)
			{
				clearChildren(node->nodes[i]);
				delete node->nodes[i];
				node->nodes[i] = nullptr;
			}
		}
	}
	void insertVoxelPosition(const Vector3& position, T data, Octree* currentNode, int currentDepth, int targetDepth)
	{
		if (currentDepth == targetDepth)
		{
			currentNode->isLeaf = true;
			currentNode->value = data;
			return;
		}
		for (int i = 0; i < maxChildren; ++i)
		{
			if (VectorMath::isInBox(position, currentNode->center + offsets[i] * currentNode->size * 0.25f, currentNode->size * 0.25f))
			{
				if (currentNode->nodes[i] == nullptr)
				{
					currentNode->AddNode(i);
				}
				return insertVoxelPosition(position, data, currentNode->nodes[i], ++currentDepth, targetDepth);
			}
		}
	}

	Octree* AddNode(uint8_t index, bool leaf = false, const T& val = T{})
	{
		++count;
		
		if (nodes[index] != nullptr)
		{
			delete nodes[index];
		}

		nodes[index] = new Octree(val, center + offsets[index] * size * 0.25f, size / 2, leaf);

		return nodes[index];
	}

};
