#include "SPATApp.h"
#include "ImGuiLayer.h"
#include "OctreeLoaderManager.h"
#include "Engine.h"
#include "Window.h"

SPATApp::SPATApp(): level(scene)
{
	level.init();

	PointCloudOctreeLoader().loadTo("models/vox.ply", scene->octree);

	level.getRenderManager().updateScene();

	Engine::addRenderLayer(level.getViewport());
	Engine::addRenderLayer(std::make_shared<ImGuiLayer>(Engine::getWindow()->getGLFWwindow()));
}

void SPATApp::onUpdate()
{
	if (Input::getKeyDown(KeyCode::ESC))
		Engine::getWindow()->close();
}

void SPATApp::onGuiDraw()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open"))
			{
				openFile();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
}

void SPATApp::openFile()
{
	Logger::log("open");
	auto maybeFilePath = FileUtils::openFileDialog();
	if (maybeFilePath)
	{
		const auto path = maybeFilePath.value();
		Logger::log(path);
		const auto maybeLoader = OctreeLoaderManager::getAssociatedLoader(path);
		if (maybeLoader)
		{
			maybeLoader.value()->loadTo(path, scene->octree);
			level.getRenderManager().updateScene();
		}
		else
		{
			Logger::log("Unknown file extension");
		}
	}
	else
	{
		Logger::log("File not selected");
	}
}
