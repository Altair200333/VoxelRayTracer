#pragma once
#include "Component.h"
#include "Vector3.h"

class Transform: public Component
{
public:
	Vector3 position;
	explicit Transform(Vector3 pos = Vector3()):position(pos)
	{}
};