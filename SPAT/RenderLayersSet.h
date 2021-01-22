#pragma once
#include <memory>
#include <vector>
#include "RenderLayer.h"

class RenderLayersSet final
{
public:
	std::vector<std::shared_ptr<RenderLayer>> layers;

	void render()
	{
		for (auto& layer : layers)
		{
			layer->onBegin();
			layer->onDraw();
			layer->onEnd();
		}
	}
	void add(std::shared_ptr<RenderLayer> layer)
	{
		layers.push_back(layer);
	}
};
