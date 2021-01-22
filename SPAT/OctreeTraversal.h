#pragma once
#include <iostream>


#include "Octree.h"
#include "Ray.h"
#include "Vector3.h"

template<typename T>
class OctreeTraversal
{
	
public:
	virtual ~OctreeTraversal() = default;

	struct Contact final
	{
		bool hit = false;
		T value{};
		Vector3 pos{};
		Vector3 normal{};
	};
	
	Contact hit(const Ray& ray, Octree<T>& octree)
	{
		Octree<T>* node = hitNode(ray, octree);
		if (node != nullptr)
		{
			auto [hitPos, normal] = VectorMath::cubeHit(ray.direction, ray.origin, node->center, node->size * 0.5f);
			return { true, node->value, hitPos, normal};
		}
		return { false };
	}
	
protected:
	virtual Octree<T>* hitNode(const Ray& r, Octree<T>& octree) = 0;
	
	[[nodiscard]] bool AABBTest(const Ray& r, Octree<T>& octree) const
	{
		float t1 = (octree.bounds[0].x - r.origin.x) * r.inverseDir.x;
		float t2 = (octree.bounds[1].x - r.origin.x) * r.inverseDir.x;

		float tmin = std::min(t1, t2);
		float tmax = std::max(t1, t2);

		t1 = (octree.bounds[0].y - r.origin.y) * r.inverseDir.y;
		t2 = (octree.bounds[1].y - r.origin.y) * r.inverseDir.y;

		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));

		t1 = (octree.bounds[0].z - r.origin.z) * r.inverseDir.z;
		t2 = (octree.bounds[1].z - r.origin.z) * r.inverseDir.z;

		tmin = std::max(tmin, std::min(t1, t2));
		tmax = std::min(tmax, std::max(t1, t2));

		return tmax > std::fmax(tmin, 0.0);
	}
};