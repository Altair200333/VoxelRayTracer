#include "OpenGLRenderer.h"
#include "SkyBackground.h"
#include "Logger.h"
#include "FileUtils.h"
#include "GLSLOctreeSerializer.h"

void OpenGLRenderer::loadTexel(const std::vector<float>& buffer)
{
	unsigned int texture1;
	glGenBuffers(1, &texture1);
	glBindBuffer(GL_TEXTURE_BUFFER, texture1);
	glBufferData(GL_TEXTURE_BUFFER, buffer.size(), buffer.data(), GL_STATIC_DRAW);

	GLuint id;
	glGenTextures(1, &id);
	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	glTexBuffer(GL_TEXTURE_BUFFER, GL_RGBA32F, texture1);

	glBindBuffer(GL_TEXTURE_BUFFER, 0);
	glBindTexture(GL_TEXTURE_BUFFER, 0);
}

OpenGLRenderer::OpenGLRenderer(std::shared_ptr<Scene> scene): SceneRenderer(scene, "GPU OpenGL renderer")
{
	shader = Shader(FileUtils::fileContents("Shaders/renderShader.vs"),
	                FileUtils::fileContents("Shaders/renderShader.fs"), FileUtils::fileContents("Shaders/geometry.gs"));

	//create dummy vao
	glGenVertexArrays(1, &VAO);
	shader.use();

	loadTexel(GLSLOctreeSerializer().toFloatArray(scene->octree));
}

void OpenGLRenderer::uploadCamera(std::optional<RenderContext> params) const
{
	shader.setVec3("origin", params->transform->position);
	shader.setVec3("front", params->camera->front);
	shader.setVec3("right", params->camera->right);
	shader.setVec3("up", params->camera->up);
	shader.setFloat("fov", params->camera->fov);
}

void OpenGLRenderer::uploadRenderParams(ResizeableTexture& outTex, std::optional<RenderContext> params) const
{
	const float closeHeight = 2 * tan(params->camera->fov / 2 * 3.14159265 / 180);
	const float scale = closeHeight / outTex.height;
	shader.setFloat("scale", scale);
	shader.setFloat("width", outTex.width);
	shader.setFloat("height", outTex.height);
}

void OpenGLRenderer::uploadLight(std::optional<RenderContext> params) const
{
	shader.setFloat("dirLight.lightDist", 200.0f);
	shader.setVec3("dirLight.dir", params->light->getDirection());
	shader.setVec3("dirLight.color", params->light->color);
	shader.setFloat("dirLight.strength", params->light->strength);
}

void OpenGLRenderer::uploadSkyBackground(const std::shared_ptr<SkyBackground>& background) const
{
	shader.setVec3("world.skyColor", background->skyColor);
	shader.setVec3("world.floorColor", background->floorColor);
	shader.setVec3("world.horizonColor", background->horizonColor);
	shader.setFloat("world.horizonAngle", background->horizonAngle);
}

bool OpenGLRenderer::uploadValues(ResizeableTexture& outTex) const
{
	const auto params = getRenderContext();
	if (!params)
	{
		Logger::log("Transform and camera components are required for rendering");
		return false;
	}

	uploadCamera(params);
	uploadRenderParams(outTex, params);
	uploadLight(params);

	const std::shared_ptr<SkyBackground> skyBackground = std::dynamic_pointer_cast<SkyBackground>(
		scene->world.backgroundManager.getActiveBackground());
	if (skyBackground)
	{
		uploadSkyBackground(skyBackground);
		shader.setInt("backType", 0);
	}
	else
	{
		shader.setInt("backType", 1);
	}

	return true;
}

void OpenGLRenderer::previewRender(ResizeableTexture& outTex)
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, outTex.width, outTex.height);

	shader.use();

	if (!uploadValues(outTex))
	{
		Logger::log("Unable to upload values to shader");
		return;
	}

	glBindVertexArray(VAO);
	glDrawArrays(GL_POINTS, 0, 1);

	//Tell OpenGL that we want to read 3 component(not 4 which is defaulted)
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(0, 0, outTex.width, outTex.height, GL_RGB, GL_UNSIGNED_BYTE, outTex.data());

	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
}

void OpenGLRenderer::onGuiDraw()
{
	ImGui::Text("OpenGL renderer");
}

void OpenGLRenderer::updateScene()
{
	loadTexel(GLSLOctreeSerializer().toFloatArray(scene->octree));
}
