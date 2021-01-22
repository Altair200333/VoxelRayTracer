#include "Viewport.h"

#include "ImGui/imgui.h"
#include "CameraController.h"
#include "Logger.h"

void Viewport::setParams()
{
	// Setup filtering parameters for display
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	// This is required on WebGL for non power-of-two textures
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same
}

Viewport::Viewport(): texture(width, height)
{
	bindGLTexture();
	texture.reset();
	glGenTextures(1, &viewportTexture);
}

void Viewport::onBegin()
{
	Profiler::startFrame();
}

void Viewport::onEnd()
{
	Profiler::endFrame();
}

void Viewport::onDraw()
{
	auto camera = renderer->getScene()->getCamera().getComponent<CameraController>();
	if (!lock && wasResized || (camera && camera.value()->hasMoved))
		texture.reset();

	if (renderer && !lock)
		renderer->previewRender(texture);
	else
		Logger::log("Renderer not specified");

	bindGLTexture();

	if (wasResized)
		texture.resize(width, height);
}

void Viewport::onGuiDraw()
{
	ImGui::Begin("Viewport");
	{
		ImGui::BeginChild("Viewport");

		const ImVec2 windowSize = ImGui::GetWindowSize();

		wasResized = static_cast<int>(windowSize.x) != width || static_cast<int>(windowSize.y) != height;

		width = static_cast<int>(windowSize.x);
		height = static_cast<int>(windowSize.y);

		ImGui::Image(reinterpret_cast<ImTextureID>(viewportTexture), windowSize, ImVec2(0, 1), ImVec2(1, 0));

		ImGui::EndChild();
	}
	ImGui::End();
}

void Viewport::bindGLTexture()
{
	glBindTexture(GL_TEXTURE_2D, viewportTexture);

	setParams();

	// Upload pixels into texture
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, texture.width, texture.height, 0, GL_RGB, GL_UNSIGNED_BYTE, texture.data());

	glBindTexture(GL_TEXTURE_2D, 0);
}
