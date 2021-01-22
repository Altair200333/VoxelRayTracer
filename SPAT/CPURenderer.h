#pragma once

#include <future>
#include <random>
#include <utility>

#include "DirectionalLight.h"
#include "Logger.h"
#include "OctreeTraversal.h"
#include "RevellesTraversal.h"
#include "SceneRenderer.h"
#include "TextureRegion.h"

class CPURenderer: public SceneRenderer
{
protected:
	int samples = 5;

	static Ray getShadowRay(const OctreeTraversal<Vector3>::Contact& contact,
	                        const std::shared_ptr<DirectionalLight>& light);

	Vector3 castRay(const Vector3& from, const Vector3& direction, const WorldSettings& world,
	                const RenderContext& context, Octree<Vector3>& octree) const;

	void tracePixel(const TextureRegion& region, const RenderContext& params, size_t posX, size_t posY) const;
public:
	
	explicit CPURenderer(std::shared_ptr<Scene> scene, const std::string& _name):SceneRenderer(std::move(scene), _name){}

	void onGuiDraw() override;

	virtual void renderRegion(const TextureRegion& region, const RenderContext& params) = 0;

	void previewRender(ResizeableTexture& texture) override;

	[[nodiscard]] size_t getThreadsCount() const;

	template<typename PerRegionRoutine>
	void multiThreadedRender(ResizeableTexture& texture, PerRegionRoutine routine)
	{
		auto maybeParams = getRenderContext();
		if (!maybeParams)
		{
			Logger::log("Transform and camera components are required for rendering");
			return;
		}
		auto params = maybeParams.value();

		const auto processor_count = getThreadsCount();
		auto regions = TextureRegion::splitTextureIntoRegions(texture, processor_count);
		
		std::vector<std::future<void>> calls;
		for (size_t i = 0; i < processor_count; ++i)
		{
			calls.emplace_back(std::async(std::launch::async, [&params, &routine](TextureRegion region)
				{
					routine(region, params);
				}, regions[i]));
		}

		for (auto&& fut : calls)
			fut.get();
	}

	void fullscaleRender(ResizeableTexture& texture) override;

	void updateScene() override{}
};
