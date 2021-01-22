#pragma once

#include "SceneRenderer.h"
#include "Shader.h"
#include "SkyBackground.h"

class OpenGLRenderer final: public SceneRenderer
{
	Shader shader;

	//actually useless VAO. we just have to render something
    unsigned int VAO = 0;
public:
	//upload so called texture buffer object to opengl
	static void loadTexel(const std::vector<float>& buffer);


	explicit OpenGLRenderer(std::shared_ptr<Scene> scene);

	void uploadCamera(std::optional<RenderContext> params) const;

	void uploadRenderParams(ResizeableTexture& outTex, std::optional<RenderContext> params) const;

	void uploadLight(std::optional<RenderContext> params) const;

	void uploadSkyBackground(const std::shared_ptr<SkyBackground>& background) const;

	bool uploadValues(ResizeableTexture& outTex) const;

	void previewRender(ResizeableTexture& outTex) override;

	void fullscaleRender(ResizeableTexture& texture) override
	{
		
	}

	void onGuiDraw() override;

	void updateScene() override;
};
