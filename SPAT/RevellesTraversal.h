#pragma once
#include <vector>

#include "Octree.h"
#include "OctreeTraversal.h"
#include "Vector3.h"

//based on Revelles's octree traversal paper and this https://github.com/Forceflow/cpu_voxel_raycaster code
//Performance of this algorithm is not that great
//It turns out that on my octrees this algorithm is not any faster than naive alg
//but this code is substantially more complicated than naive one
template<typename T>
class RevellesTraversal final : public OctreeTraversal<T>
{
	using Node = Octree<T>;

	struct NodeTraversalInfo final
	{
		Node* node = nullptr;
		Vector3 t0{}, t1{}, tm{};
		int nextChild = -1;
	};
	
	uint8_t nodeMask = 0;
	Node* rootNode = nullptr;
	Ray ray{};
	Ray original_ray{};
	int stepsCount = 0;

	std::vector<NodeTraversalInfo> stack;

public:
	RevellesTraversal() = default;
	
protected:
	Octree<Vector3>* hitNode(const Ray& r, Octree<Vector3>& octree) override
	{
		this->rootNode = &octree;
		this->ray = r;
		
		initTraversal();

		while (!isTerminated())
		{
			const auto node = getCurrentNode();
			//we need to test if the original ray intersects node we just found
			//otherwise rays can collide with mirrored octree
			if (node->isLeaf && OctreeTraversal<T>::AABBTest(original_ray, *node))
			{
				return node;
			}
			//just make a step
			step();
		}
		//we didn't find anything
		return nullptr;
	}
	
	void initTraversal()
	{
		stepsCount = 0;
		nodeMask = 0;

		original_ray = ray;

		//This algorithm works only for rays with positive directions so we need to mirror ray if goes in negative direction 
		if (ray.direction[0] < 0.0f) 
		{
			ray.origin[0] = rootNode->center[0] - ray.origin[0];
			ray.direction[0] = -ray.direction[0];
			ray.inverseDir[0] = -ray.inverseDir[0];
			nodeMask |= 4; //bitwise OR (latest bits are XYZ)
		}
		if (ray.direction[1] < 0.0f) 
		{
			ray.origin[1] = rootNode->center[1]- ray.origin[1];
			ray.direction[1] = -ray.direction[1];
			ray.inverseDir[1] = -ray.inverseDir[1];
			nodeMask |= 2;
		}
		if (ray.direction[2] < 0.0f) 
		{
			ray.origin[2] = rootNode->center[2]  - ray.origin[2];
			ray.direction[2] = -ray.direction[2];
			ray.inverseDir[2] = -ray.inverseDir[2];
			nodeMask |= 1;
		}

		const float tx0 = (rootNode->bounds[0][0] - ray.origin[0]) * ray.inverseDir[0];
		const float tx1 = (rootNode->bounds[1][0] - ray.origin[0]) * ray.inverseDir[0];
		const float ty0 = (rootNode->bounds[0][1] - ray.origin[1]) * ray.inverseDir[1];
		const float ty1 = (rootNode->bounds[1][1] - ray.origin[1]) * ray.inverseDir[1];
		const float tz0 = (rootNode->bounds[0][2] - ray.origin[2]) * ray.inverseDir[2];
		const float tz1 = (rootNode->bounds[1][2] - ray.origin[2]) * ray.inverseDir[2];

		//push node info on the stack if we intersect root node i.e. bounding box of the scene
		if (std::max({ tx0, ty0, tz0 }) < std::min({tx1, ty1, tz1})) 
		{
			stack.push_back(buildNodeInfo(tx0, ty0, tz0, tx1, ty1, tz1, rootNode));
		}
	}

	static NodeTraversalInfo buildNodeInfo(float tx0, float ty0, float tz0, float tx1, float ty1, float tz1, Node* node)
	{
		NodeTraversalInfo info;
		info.node = node;
		info.t0 = Vector3(tx0, ty0, tz0);
		info.t1 = Vector3(tx1, ty1, tz1);
		info.nextChild = -1;
		return info;
	}

	static int firstNode(float tx0, float ty0, float tz0, float txm, float tym, float tzm)
	{
		unsigned char answer = 0;	// initialize to 00000000
		// select the entry plane and set bits
		if (tx0 > ty0) 
		{
			if (tx0 > tz0) // PLANE YZ
			{ 
				if (tym < tx0) answer |= 2;	// set bit at position 1
				if (tzm < tx0) answer |= 1;	// set bit at position 0
				return static_cast<int>(answer);
			}
		}
		else 
		{
			if (ty0 > tz0) // PLANE XZ
			{ 
				if (txm < ty0) answer |= 4;	// set bit at position 2
				if (tzm < ty0) answer |= 1;	// set bit at position 0
				return static_cast<int>(answer);
			}
		}
		// PLANE XY
		if (txm < tz0) answer |= 4;	// set bit at position 2
		if (tym < tz0) answer |= 2;	// set bit at position 1
		return static_cast<int>(answer);
	}

	static int newNode(float txm, int x, float tym, int y, float tzm, int z)
	{
		if (txm < tym) 
		{
			if (txm < tzm) { return x; }  // YZ plane
		}
		else 
		{
			if (tym < tzm) { return y; } // XZ plane
		}
		return z; // XY plane;
	}

	[[nodiscard]] bool isTerminated() const
	{
		return stack.empty();
	}
	Node* getCurrentNode()
	{
		return stack.back().node;
	}

	//move one step ahead along the ray and test underlying nodes
	void step()
	{
		stepsCount++;
		// if the stack is empty, we're done
		if (stack.empty())
			return;

		Vector3& t0 = stack.back().t0;
		Vector3& t1 = stack.back().t1;
		Vector3& tm = stack.back().tm;

		//POP
		// if we're in a terminal node or if we visited all children of that node (next child = 8), 
		// or if the t1 values have become negative, pop the current node from the stack
		if (stack.back().nextChild == 8 || stack.back().node->isLeaf)
		{
			stack.pop_back();
			return;
		}
		auto& backNode = stack.back();

		// PUSH
		// we haven't looked at any child in this voxel yet: pick the first and push down
		if (backNode.nextChild == -1)
		{
			// calculate midpoint and save it in stack
			tm = (t0 + t1) * 0.5f;
			// calculate first node
			backNode.nextChild = firstNode(t0[0], t0[1], t0[2], tm[0], tm[1], tm[2]);
		}

		// ADVANCE
		// let's look at the next child in this voxel
		switch (backNode.nextChild)
		{
		case 0:
			{
				backNode.nextChild = newNode(tm[0], 4, tm[1], 2, tm[2], 1);
				if (backNode.node->nodes[nodeMask]) 
				{
					NodeTraversalInfo info = buildNodeInfo(t0[0], t0[1], t0[2], tm[0], tm[1], tm[2], backNode.node->nodes[nodeMask]);
					stack.push_back(info);
				}
				return;
			}
		case 1:
			{
				backNode.nextChild = newNode(tm[0], 5, tm[1], 3, t1[2], 8);
				if (backNode.node->nodes[1 ^ nodeMask])
				{
					NodeTraversalInfo info = buildNodeInfo(t0[0], t0[1], tm[2], tm[0], tm[1], t1[2], backNode.node->nodes[1 ^ nodeMask]);
					stack.push_back(info);
				}
				return;
			}
		case 2:
			{
				backNode.nextChild = newNode(tm[0], 6, t1[1], 8, tm[2], 3);
				if (backNode.node->nodes[2 ^ nodeMask]) 
				{
					NodeTraversalInfo info = buildNodeInfo(t0[0], tm[1], t0[2], tm[0], t1[1], tm[2], backNode.node->nodes[2 ^ nodeMask]);
					stack.push_back(info);
				}
				return;
			}
		case 3:
			{
				backNode.nextChild = newNode(tm[0], 7, t1[1], 8, t1[2], 8);
				if (backNode.node->nodes[3 ^ nodeMask])
				{
					NodeTraversalInfo info = buildNodeInfo(t0[0], tm[1], tm[2], tm[0], t1[1], t1[2], backNode.node->nodes[3 ^ nodeMask]);
					stack.push_back(info);
				}
				return;
			}
		case 4:
			{
				backNode.nextChild = newNode(t1[0], 8, tm[1], 6, tm[2], 5);
				if (backNode.node->nodes[4 ^ nodeMask]) 
				{
					NodeTraversalInfo info = buildNodeInfo(tm[0], t0[1], t0[2], t1[0], tm[1], tm[2], backNode.node->nodes[4 ^ nodeMask]);
					stack.push_back(info);
				}
				return;
			}
		case 5:
			{
				backNode.nextChild = newNode(t1[0], 8, tm[1], 7, t1[2], 8);
				if (backNode.node->nodes[5 ^ nodeMask]) {
					NodeTraversalInfo info = buildNodeInfo(tm[0], t0[1], tm[2], t1[0], tm[1], t1[2], backNode.node->nodes[5 ^ nodeMask]);
					stack.push_back(info);
				}
				return;
			}
		case 6:
			{
				backNode.nextChild = newNode(t1[0], 8, t1[1], 8, tm[2], 7);
				if (backNode.node->nodes[6 ^ nodeMask]) {
					NodeTraversalInfo info = buildNodeInfo(tm[0], tm[1], t0[2], t1[0], t1[1], tm[2], backNode.node->nodes[6 ^ nodeMask]);
					stack.push_back(info);
				}
				return;
			}
		case 7:
			{
				backNode.nextChild = 8;
				if (backNode.node->nodes[7 ^ nodeMask]) 
				{
					NodeTraversalInfo info = buildNodeInfo(tm[0], tm[1], tm[2], t1[0], t1[1], t1[2], backNode.node->nodes[7 ^ nodeMask]);
					stack.push_back(info);
				} return;
			}
		default: return;
		}
	}
};
