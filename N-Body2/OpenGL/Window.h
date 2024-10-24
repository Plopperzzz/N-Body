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

	static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			if (key == GLFW_KEY_W || key == GLFW_KEY_S || key == GLFW_KEY_A || key == GLFW_KEY_D) {

				//	if (key == GLFW_KEY_W) {
				//		Position += speed * Up; // Move up
				//	}
				//	if (key == GLFW_KEY_S) {
				//		Position -= speed * Up; // Move down
				//	}
				//	if (key == GLFW_KEY_A) {
				//		Position -= speed * glm::normalize(glm::cross(Orientation, Up)); // Move left
				//	}
				//	if (key == GLFW_KEY_D) {
				//		Position += speed * glm::normalize(glm::cross(Orientation, Up)); // Move right
				//	}
				//// Update orthographic projection after panning
				//SetOrthographic(orthoLeft + Position.x, orthoRight + Position.x,
				//	orthoBottom + Position.y, orthoTop + Position.y,
				//	nearPlane, farPlane);
			}
		}
		if (key == GLFW_KEY_SPACE)
		{
			Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
			cam->debug = !cam->debug;
			return;
		}
	}

	// Mouse button callback to handle middle mouse button press
	static void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
		Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		if (button == GLFW_MOUSE_BUTTON_MIDDLE) {
			if (action == GLFW_PRESS) {
				cam->middleMousePressed = true;
				double xpos, ypos;
				glfwGetCursorPos(window, &xpos, &ypos);
				cam->lastMousePosition = glm::vec2(xpos, ypos);
			}
			else if (action == GLFW_RELEASE) {
				cam->middleMousePressed = false;
			}
		}
	}

};
#endif