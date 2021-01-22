#pragma once
#include "Camera.h"
#include "Object.h"
#include "Octree.h"

#include "WorldSettings.h"

class Scene final: public GuiDrawable
{
public:
	WorldSettings world;
	
	Octree<Vector3> octree;
	Object camera;

	explicit Scene();

	[[nodiscard]] auto& getCamera() const
	{
		return camera;
	}

	void onGuiDraw() override;
};
