#pragma once
#include "OctreeTraversal.h"
template<typename T>
class NaiveOctreeTraversal final : public OctreeTraversal<T>
{
protected:
	Octree<T>* hitNode(const Ray& ray, Octree<T>& octree) override
	{
		std::deque<Octree<T>*> buffer;
		buffer.push_back(&octree);
		
		Octree<T>* closestNode = nullptr;
		
		while (!buffer.empty())
		{
			Octree<T>* currentNode = buffer.front();
			buffer.pop_front();

			if (currentNode->isLeaf && (closestNode == nullptr || (currentNode->center.squareDist(ray.origin) <= closestNode->center.squareDist(ray.origin))))
			{
				closestNode = currentNode;
			}

			std::for_each(std::begin(currentNode->nodes), std::end(currentNode->nodes), [&buffer, &ray](Octree<T>* childNode)
			{
				if (childNode != nullptr && OctreeTraversal<T>::AABBTest(ray, *childNode))
				{
					buffer.push_back(childNode);
				}
			});
		}
		
		return closestNode;
	}

};
