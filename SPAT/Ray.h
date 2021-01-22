#pragma once
#include <cstdint>

#include "Vector3.h"

struct Ray final
{	
	Vector3 origin{}, direction{};
	Vector3 inverseDir{};
	std::array<uint8_t, 3> sign{};

	Ray() = default;
	Ray(const Vector3& _orig, const Vector3& _dir) : origin(_orig), direction(_dir)
	{
		inverseDir = { 1.0f / direction.x,1.0f / direction.y,1.0f / direction.z };
		sign[0] = (inverseDir.x < 0);
		sign[1] = (inverseDir.y < 0);
		sign[2] = (inverseDir.z < 0);
	}
};
