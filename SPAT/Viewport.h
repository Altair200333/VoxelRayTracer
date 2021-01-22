#pragma once
#include <glad/glad.h>


#include "RenderLayer.h"
#include "ResizeableTexture.h"
#include "SceneRenderer.h"

class Viewport final: public RenderLayer, public GuiDrawable
{
	GLuint viewportTexture = 0;

	int width = 1000, height= 1000;
	ResizeableTexture texture;

	bool wasResized = false;
	bool lock = false;

	static void setParams();

	void bindGLTexture();
public:
	std::shared_ptr<SceneRenderer> renderer = nullptr;
	
	bool& getLock()
	{
		return lock;
	}

	[[nodiscard]] int getWidth() const
	{
		return width;
	}

	[[nodiscard]] int getHeight() const
	{
		return height;
	}
	auto& getTexture()
	{
		return texture;
	}

	explicit Viewport();

	void onBegin() override;

	void onEnd() override;

	void onDraw() override;

	void onGuiDraw() override;
};
