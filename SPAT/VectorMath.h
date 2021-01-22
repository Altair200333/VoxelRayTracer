#pragma once
#include <cstdint>
#include <cmath>
#include "Vector3.h"

namespace VectorMath
{
	inline Vector3 intersectPoint(const Vector3& rayVector, const Vector3& rayPoint, const Vector3& planeNormal, const Vector3& planePoint)
	{
		const float t = (planePoint - rayPoint).dot(planeNormal) / planeNormal.dot(rayVector);
		return rayPoint + rayVector * t;
	}

	inline bool isInBox(const Vector3& point, const Vector3& boxPos, float halfWidth)
	{
		constexpr float epsilon = 0.0001f;
		return std::fabs(point.x - boxPos.x) <= halfWidth + epsilon && std::fabs(point.y - boxPos.y) <= halfWidth + epsilon && std::fabs(point.z - boxPos.z) <= halfWidth + epsilon;
	}
	struct Contact
	{
		Vector3 position;
		Vector3 normal;
	};

	inline Contact cubeHit(const Vector3& rayVector, const Vector3& rayPoint, const Vector3& boxPos, float halfWidth)
	{
		static const Vector3 normals[6] = {
			{ 1, 0, 0},
			{-1, 0, 0},
			{ 0, 1, 0},
			{ 0,-1, 0},
			{ 0, 0, 1},
			{ 0, 0,-1}
		};
		Vector3 closest = { 100000, 100000, 10000 };
		uint8_t normalId = -1;
		for (int i = 0; i < 6; ++i)
		{
			Vector3 pos = intersectPoint(rayVector, rayPoint, normals[i] * halfWidth, boxPos + normals[i] * halfWidth);
			if (isInBox(pos, boxPos, halfWidth) && pos.squareDist(rayPoint) < closest.squareDist(rayPoint))
			{
				normalId = i;
				closest = pos;
			}
		}
		return { closest, normals[normalId] };
	}

	inline Vector3 reflect(const Vector3& I, const Vector3& N)
	{
		return I - N * I.dot(N) * 2.0f;
	}
}
