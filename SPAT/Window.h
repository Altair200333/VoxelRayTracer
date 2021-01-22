#pragma once
#include <glad/glad.h>
#include "GLFW/glfw3.h"
#include "WindowProps.h"
#include "Logger.h"


class Window final: public OnUpdateSubscriber
{
	WindowProps properties;
	GLFWwindow* window{};

	static void initGLFW()
	{
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}
public:
	bool running = true;

	void createWindow(const WindowProps& props);
	explicit Window(const WindowProps& props);
	
	void onUpdate() override;
	static void clear();
	[[nodiscard]] GLFWwindow* getGLFWwindow() const;

	void close() const;

	static void frameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void mouseCallback(GLFWwindow* window, double xPos, double yPos);
	~Window();
};

#include "Input.h"
#include "MouseInput.h"

inline void Window::createWindow(const WindowProps& props)
{
	window = glfwCreateWindow(props.width, props.height, props.name.c_str(), nullptr, nullptr);
	
	if (window == nullptr)
	{
		Logger::log("Failed to create GLFW window");
		glfwTerminate();
	}
	
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
	{
		Logger::log("Failed to initialize GLAD");
	}

	glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
	glfwSetKeyCallback(window, keyCallback);
	glfwSetCursorPosCallback(window, mouseCallback);
}

inline Window::Window(const WindowProps& props): properties(props)
{
	initGLFW();
	createWindow(props);
}

inline void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	Input::setKeyState(key, action);
}

inline void Window::mouseCallback(GLFWwindow* window, double xPos, double yPos)
{
	MouseInput::moveMouse(xPos, yPos);
}

inline void Window::onUpdate() 
{	
	running = !glfwWindowShouldClose(window);

	glfwSwapBuffers(window);
	glfwPollEvents();
}

inline void Window::clear()
{
	glClearColor(0.2f, 0.2f, 0.22f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
}

inline GLFWwindow* Window::getGLFWwindow() const
{
	return window;
}

inline void Window::close() const
{
	glfwSetWindowShouldClose(window, GLFW_TRUE);
}

inline Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}
