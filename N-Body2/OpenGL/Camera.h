#pragma once
#ifndef CAMERA_CLASS_H
#define CAMERA_CLASS_H

#define GLM_ENABLE_EXPERIMENTAL // Enable experimental extensions
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "Shader.h"

class Camera
{
public:
	// Stores the main vectors of the camera
	glm::vec3 Position;
	glm::vec3 Orientation;
	glm::vec3 Up;
	glm::vec2 lastMousePosition;

	// Prevents the camera from jumping around when first clicking left click
	bool firstClick = true;
	bool middleMousePressed = false;

	// Stores the width and height of the window
	int width;
	int height;

	// Adjust the speed of the camera and it's sensitivity when looking around
	float speed;
	float sensitivity;
	float zoomLevel;

	// Projection Parameters
	bool isOrthographic;
	float orthoLeft;
	float orthoRight;
	float orthoBottom;
	float orthoTop;
	float nearPlane;
	float farPlane;
	float FOVdeg;

	// Camera constructor to set up initial values
	Camera(int width, int height, glm::vec3 position);

	// Set Perspective Projection Parameters
	void SetPerspective(float FOVdeg, float nearPlane, float farPlane);

	// Set Orthographic Projection Parameters
	void SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane);

	// Updates and exports the camera matrix to the Vertex Shader
	void Matrix(Shader& shader, const char* uniform);

	// Handles camera inputs
	void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

	void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset);

	void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

	void CursorPositionCallback(GLFWwindow* window, double xpos, double ypos);
};
#endif
