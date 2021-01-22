#include "CPURenderer.h"

#include "JPGTextureSaver.h"

Ray CPURenderer::getShadowRay(const OctreeTraversal<Vector3>::Contact& contact,
                              const std::shared_ptr<DirectionalLight>& light)
{
	static std::default_random_engine generator;
	const std::uniform_real_distribution<float> distribution(-1, 1);

	const Vector3 shadowContactPos = contact.pos + contact.normal * 0.001f;

	const Vector3 lightPos = shadowContactPos - light->getDirection() * light->distanceFromWorld;
	const Vector3 offset = {distribution(generator), distribution(generator), distribution(generator)};
	const Vector3 newLightPos = lightPos + offset * light->area;

	return Ray(shadowContactPos, newLightPos - shadowContactPos);
}

Vector3 CPURenderer::castRay(const Vector3& from, const Vector3& direction, const WorldSettings& world,
                             const RenderContext& context, Octree<Vector3>& octree) const
{
	const auto hitResult = RevellesTraversal<Vector3>().hit({from, direction}, scene->octree);

	if (hitResult.hit)
	{
		Vector3 color = context.light->multiplyColors(hitResult.value) * context.light->strength * std::fabs(
			(hitResult.normal.dot(context.light->getDirection())));

		const auto shadowHit = RevellesTraversal<Vector3>().hit(getShadowRay(hitResult, context.light), scene->octree);
		if (shadowHit.hit)
			color = color * 0.1f;

		return color;
	}

	return world.backgroundManager.getActiveBackground()->getBackgroundColor(from, direction);
}

void CPURenderer::tracePixel(const TextureRegion& region, const RenderContext& params, size_t posX, size_t posY) const
{
	const Vector3 ray = params.camera->getRayDirectionAtPixel(posX, posY, region.texture.width, region.texture.height);

	const Vector3 color = castRay(params.transform->position, ray, scene->world, params, scene->octree);

	region.texture.write(color * 255.0f, posX, posY);
}

void CPURenderer::onGuiDraw()
{
	ImGui::DragInt("Per pixel sampling", &samples, 0.1f, 1, 20);
}

void CPURenderer::previewRender(ResizeableTexture& texture)
{
	multiThreadedRender(texture, [this](TextureRegion& region, RenderContext& params)
	{
		renderRegion(region, params);
	});
}

size_t CPURenderer::getThreadsCount() const
{
	const auto processor_count = std::thread::hardware_concurrency();
	return std::max(1u, processor_count);
}

void CPURenderer::fullscaleRender(ResizeableTexture& texture)
{
	multiThreadedRender(texture, [this, &texture](TextureRegion& region, RenderContext& params)
	{
		for (int y = 0; y < texture.height; ++y)
		{
			for (int x = region.from; x < region.to; ++x)
			{
				Vector3 totalColor;
				for (float subPixelX = 0; subPixelX < samples; ++subPixelX)
				{
					for (float subPixelY = 0; subPixelY < samples; ++subPixelY)
					{
						Vector3 ray = params.camera->getRayDirectionAtPixel(
							x + subPixelX / samples - 0.5f, y + subPixelY / samples - 0.5f,
							texture.width, texture.height);

						Vector3 color = castRay(params.transform->position, ray, scene->world, params, scene->octree);
						totalColor = totalColor + color;
					}
				}

				texture.write(totalColor * (255.0f / (samples * samples)), x, y);
			}
		}
	});
	JPGTextureSaver().save(texture, "f.jpg");
}
