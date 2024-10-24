#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include <unordered_map>
#include <stdexcept>

class Window
{
public:

	GLFWwindow* window;
	std::string title;
	unsigned int width;
	unsigned int height;
	unsigned int viewportWidth;
	unsigned int viewportHeight;



private:
	std::unordered_map<std::string, void*> m_variables;
	unsigned int m_swapInterval;

public:
	Window(std::string title, unsigned int width, unsigned int height, unsigned int swapInterval, unsigned int major, unsigned int minor);
	~Window();
	
	void setWindowVariable(std::string varName, void* var);
	void processInput();
	void mainLoop();
	
	void setSwapInterval(unsigned int newSwapInterval);

	void* getWindowVariable(std::string varName);

private:
	void initGLFW(unsigned int major, unsigned int minow);
	void createWindow();
	void cleanup();

};
#endif