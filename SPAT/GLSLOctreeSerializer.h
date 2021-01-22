#pragma once
#include <vector>

#include "Octree.h"

//turns Octree<Vector3> into array of floats
class GLSLOctreeSerializer final
{
	std::vector<float> buffer;
	//packPos is where is now pointer to the front-most object
	size_t currentCursorPosition = 0;

	//how many floats does it take to store octree node
	size_t OctreeNodeSize = 24;

	//we need to resize buffer manually because on write cursor jumps by <OctreeNodeSize>
	void resizeBuffer();

	size_t writeNodeParams(Octree<Vector3>* tree, size_t pos);

	//convert tree to array
	//each node in array is packed in this way
	/*Node [position.xyz][size][color.xyz][lowerBound.xyz][upperBound.xyz][childrenPositionsInArray x8][isLeafNode][isNodeEmpty]*/
	void packToArray(Octree<Vector3>* tree);

	size_t writeVector3(const Vector3& data, size_t position);
public:

	std::vector<float> toFloatArray(Octree<Vector3>& tree);
};
