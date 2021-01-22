#include "GLSLOctreeSerializer.h"
#include <glm/vec4.hpp>

void GLSLOctreeSerializer::resizeBuffer()
{
	if (currentCursorPosition + OctreeNodeSize >= buffer.size())
	{
		buffer.resize((currentCursorPosition + OctreeNodeSize) * 2, 0);
	}
}

size_t GLSLOctreeSerializer::writeNodeParams(Octree<Vector3>* tree, size_t pos)
{
	pos = writeVector3(tree->center, pos);

	buffer[pos++] = tree->size;

	pos = writeVector3(tree->value, pos);
	pos = writeVector3(tree->bounds[0], pos);
	pos = writeVector3(tree->bounds[1], pos);

	return pos;
}

void GLSLOctreeSerializer::packToArray(Octree<Vector3>* tree)
{
	size_t offsetInNode = currentCursorPosition;
	resizeBuffer();
	offsetInNode = writeNodeParams(tree, offsetInNode);

	for (size_t i = 0; i < 8; ++i)
	{
		if (tree->nodes[i] != nullptr)
		{
			currentCursorPosition += OctreeNodeSize;
			buffer[offsetInNode + i] = currentCursorPosition;
			packToArray(tree->nodes[i]);
		}
		else
		{
			buffer[offsetInNode + i] = -1;
		}
	}

	offsetInNode += 8; //skip 8 positions that are pointers to children
	buffer[offsetInNode++] = tree->isLeaf ? 1 : -1;
	buffer[offsetInNode++] = tree->count == 0 ? 1 : -1;
}

size_t GLSLOctreeSerializer::writeVector3(const Vector3& data, size_t position)
{
	buffer[position++] = data.x;
	buffer[position++] = data.y;
	buffer[position++] = data.z;

	return position;
}

std::vector<float> GLSLOctreeSerializer::toFloatArray(Octree<Vector3>& tree)
{
	buffer.clear();
	currentCursorPosition = 0;
	packToArray(&tree);
	buffer.resize(buffer.size() * sizeof(glm::vec4));

	return buffer;
}
