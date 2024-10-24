#include "Window.h"

Window::Window(std::string title, unsigned int width, unsigned int height, unsigned int swapInterval = 0, unsigned int major=3, unsigned int minor = 3):
	title(title),
	width(width),
	height(height),
	m_swapInterval(swapInterval),
	m_variables()
{
	// Initialize GLFW
	initGLFW(major, minor);
	createWindow();
	setSwapInterval(swapInterval);
}

Window::~Window()
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

void Window::setWindowVariable(std::string varName, void* var)
{
	m_variables[varName] = var;
}

void Window::setSwapInterval(unsigned int newSwapInterval)
{
	m_swapInterval = newSwapInterval;
	glfwSwapInterval(newSwapInterval);
}

void* Window::getWindowVariable(std::string varName)
{
	if (m_variables.find(varName) != m_variables.end())
	{
		return nullptr;
	}
	else
		return m_variables[varName];
}

void Window::initGLFW(unsigned int major, unsigned int minor)
{
	if (!glfwInit())
	{
		glfwTerminate();
		throw std::runtime_error("Failed to initialize GLFW");
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}

void Window::createWindow()
{
	window = glfwCreateWindow(width, height, title.c_str(), NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		throw std::runtime_error("Failed to create GLFW window");
	}

	glfwMakeContextCurrent(window);
	gladLoadGL();

	glEnable(GL_PROGRAM_POINT_SIZE);

	glViewport(0, 0, width, height);
}
