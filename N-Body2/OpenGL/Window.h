#pragma once
#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>
#include <string>
#include <unordered_map>

class Window
{
public:

	GLFWwindow* window;
	int width;
	int height;
	int viewportWidth;
	int viewportHeight;
	std::string title;

private:
	std::unordered_map<std::string, void*> variables;

public:
	Window(std::string title, int width, int height);
	~Window();
	
	void setWindowVariable(std::string varName, void* var);
	void processInput();
	void mainLoop();
	
	void* getWindowVariable(std::string varName);

private:
	void initGLFW();
	void createWindow();
	void cleanup();

};
#endif