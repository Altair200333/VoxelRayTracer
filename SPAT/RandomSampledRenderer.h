#pragma once
#include <future>
#include "CPURenderer.h"
#include "SceneRenderer.h"

class RandomSampledRenderer final : public CPURenderer
{
	int sampling = 2300;
	
	static constexpr int maxSamples = 20000;
	static constexpr int minSamples = 100;
public:
	explicit RandomSampledRenderer(std::shared_ptr<Scene> scene);

	void renderRegion(const TextureRegion& region, const RenderContext& params) override;

	void onGuiDraw() override;
};
