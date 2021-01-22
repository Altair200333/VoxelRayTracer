#pragma once
#include "ResizeableTexture.h"
#include "Scene.h"
#include <string>
#include <utility>
#include "Camera.h"
#include "DirectionalLight.h"
#include "Transform.h"

class SceneRenderer: public GuiDrawable
{
protected:
	std::shared_ptr<Scene> scene;
	struct RenderContext
	{
		std::shared_ptr<Camera> camera;
		std::shared_ptr<Transform> transform;
		std::shared_ptr<DirectionalLight> light;
	};
public:
	std::string name;
	[[nodiscard]] const std::shared_ptr<Scene>& getScene() const;

	[[nodiscard]] std::optional<RenderContext> getRenderContext() const;

	SceneRenderer(std::shared_ptr<Scene> _scene, std::string _name):scene(std::move(_scene)), name(std::move(_name)){}

	virtual void updateScene() = 0;
	virtual void previewRender(ResizeableTexture& texture) = 0;
	virtual void fullscaleRender(ResizeableTexture& texture) = 0;
	
	virtual ~SceneRenderer() = default;
};
