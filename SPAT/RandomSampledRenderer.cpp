#include "RandomSampledRenderer.h"

RandomSampledRenderer::RandomSampledRenderer(std::shared_ptr<Scene> scene): CPURenderer(scene, "CPU Sampled renderer")
{
}

void RandomSampledRenderer::renderRegion(const TextureRegion& region, const RenderContext& params)
{
	static std::default_random_engine generator;

	const std::uniform_int_distribution<int> horizontalDistribution(region.from, region.to - 1);
	const std::uniform_int_distribution<int> verticalDistribution(0, region.texture.height - 1);

	for (int counter = 0; counter < sampling; ++counter)
	{
		const int randY = verticalDistribution(generator);
		const int randX = horizontalDistribution(generator);
		tracePixel(region, params, randX, randY);
	}
}

void RandomSampledRenderer::onGuiDraw()
{
	CPURenderer::onGuiDraw();
	ImGui::DragInt("Viewport sampling", &sampling, 10, minSamples, maxSamples);
	sampling = std::clamp(sampling, minSamples, maxSamples);
}
