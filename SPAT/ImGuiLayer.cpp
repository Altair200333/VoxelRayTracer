#include "ImGuiLayer.h"

#include "EventDispatcher.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui.h"
#include "ImGui/imgui_impl_glfw.h"
#include "GuiEvent.h"

void ImGuiLayer::setParams(ImGuiIO& io)
{
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}
}

void ImGuiLayer::drawContents()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::setDisplaySize(ImGuiIO& io) const
{
	int width, height;
	glfwGetWindowSize(window, &width, &height);
	io.DisplaySize = ImVec2(width, height);
}

void ImGuiLayer::updateViewports(ImGuiIO& io)
{
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		GLFWwindow* backup_current_context = glfwGetCurrentContext();
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
		glfwMakeContextCurrent(backup_current_context);
	}
}

ImGuiLayer::ImGuiLayer(GLFWwindow* _window): window(_window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	(void)io;

	setParams(io);

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init("#version 150");
}

void ImGuiLayer::onBegin()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
	ImGui::DockSpaceOverViewport(ImGui::GetMainViewport());
}

void ImGuiLayer::onEnd()
{
	ImGuiIO& io = ImGui::GetIO();;
	setDisplaySize(io);

	drawContents();

	updateViewports(io);
}

void ImGuiLayer::onDraw()
{
	EventDispatcher::dispatch(GuiEvent());
}

ImGuiLayer::~ImGuiLayer()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}
