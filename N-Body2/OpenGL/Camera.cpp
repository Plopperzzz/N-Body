#include "Camera.h"

Camera::Camera(int width, int height, glm::vec3 position)
{
	this->width = width;
	this->height = height;
	Position = position;
	Orientation = glm::vec3(0.0f, 0.0f, -1.0f);
	Up = glm::vec3(0.0f, 1.0f, 0.0f);
	firstClick = true;
	speed = 1e15f;
	sensitivity = 100.0f;
	zoomLevel = 1.0f;

	// Default to perspective projection
	isOrthographic = false;
	FOVdeg = 45.0f;
	nearPlane = 0.1f;
	farPlane = 1000.0f;

	// Default orthographic parameters
	orthoLeft = -width / 2.0f;
	orthoRight = width / 2.0f;
	orthoBottom = -height / 2.0f;
	orthoTop = height / 2.0f;

}

void Camera::SetPerspective(float FOVdeg, float nearPlane, float farPlane)
{
	this->FOVdeg = FOVdeg;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	isOrthographic = false;
}

void Camera::SetOrthographic(float left, float right, float bottom, float top, float nearPlane, float farPlane)
{
	orthoLeft = left;
	orthoRight = right;
	orthoBottom = bottom;
	orthoTop = top;
	this->nearPlane = nearPlane;
	this->farPlane = farPlane;
	isOrthographic = true;
}

void Camera::Matrix(Shader& shader, const char* uniform)
{
	// Initialize matrices
	glm::mat4 view = glm::lookAt(Position, Position + Orientation, Up);
	glm::mat4 projection;

	if (isOrthographic)
	{
		// Orthographic projection
		projection = glm::ortho(orthoLeft, orthoRight, orthoBottom, orthoTop, nearPlane, farPlane);
	}
	else
	{
		// Perspective projection
		projection = glm::perspective(glm::radians(FOVdeg), (float)width / height, nearPlane, farPlane);
	}
	// Exports the camera matrix to the Vertex Shader
	glUniformMatrix4fv(glGetUniformLocation(shader.ID, uniform), 1, GL_FALSE, glm::value_ptr(projection * view));
}

void Camera::KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {

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


void Camera::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
{
	//// Get cursor position
	//double xpos, ypos;
	//glfwGetCursorPos(window, &xpos, &ypos);

	//// Get viewport dimensions
	//int viewportWidth, viewportHeight;
	//glfwGetWindowSize(window, &viewportWidth, &viewportHeight);

	//// Convert cursor position to normalized device coordinates (-1 to 1)
	//double ndcX = (2.0 * xpos) / viewportWidth - 1.0;
	//double ndcY = 1.0 - (2.0 * ypos) / viewportHeight; // Note: Y is flipped in screen coordinates

	//// Calculate the world coordinates of the cursor position before zooming
	//double worldX = orthoLeft + (ndcX + 1.0) * (orthoRight - orthoLeft) / 2.0;
	//double worldY = orthoBottom + (ndcY + 1.0) * (orthoTop - orthoBottom) / 2.0;

	//// Adjust zoom speed
	//float zoomSpeed = 1.1f;

	//// Zoom in or out
	//if (yoffset > 0) {
	//	// Zoom in
	//	orthoLeft = worldX + (orthoLeft - worldX) / zoomSpeed;
	//	orthoRight = worldX + (orthoRight - worldX) / zoomSpeed;
	//	orthoBottom = worldY + (orthoBottom - worldY) / zoomSpeed;
	//	orthoTop = worldY + (orthoTop - worldY) / zoomSpeed;
	//	zoomLevel += 0.1 * zoomSpeed;
	//}
	//else if (yoffset < 0) {
	//	// Zoom out
	//	orthoLeft = worldX + (orthoLeft - worldX) * zoomSpeed;
	//	orthoRight = worldX + (orthoRight - worldX) * zoomSpeed;
	//	orthoBottom = worldY + (orthoBottom - worldY) * zoomSpeed;
	//	orthoTop = worldY + (orthoTop - worldY) * zoomSpeed;
	//	zoomLevel -= 0.1 / zoomSpeed;
	//}
		// Get cursor position
	double xpos, ypos;
	glfwGetCursorPos(window, &xpos, &ypos);

	// Get viewport dimensions
	int viewportWidth, viewportHeight;
	glfwGetWindowSize(window, &viewportWidth, &viewportHeight);

	// Convert cursor position to normalized device coordinates (-1 to 1)
	double ndcX = (2.0 * xpos) / viewportWidth - 1.0;
	double ndcY = 1.0 - (2.0 * ypos) / viewportHeight; // Y is flipped

	// Calculate the current view dimensions based on zoom
	float viewWidth = (orthoRight - orthoLeft) / zoomLevel;
	float viewHeight = (orthoTop - orthoBottom) / zoomLevel;

	// Calculate world coordinates relative to camera position
	float worldX = Position.x + ndcX * (viewWidth / 2.0f);
	float worldY = Position.y + ndcY * (viewHeight / 2.0f);

	// Define zoom speed
	const float zoomFactor = 1.01f;

	// Update zoom level
	if (yoffset > 0)
	{
		// Zoom in
		orthoLeft = worldX + (orthoLeft - worldX) / zoomFactor;
		orthoRight = worldX + (orthoRight - worldX) / zoomFactor;
		orthoBottom = worldY + (orthoBottom - worldY) / zoomFactor;
		orthoTop = worldY + (orthoTop - worldY) / zoomFactor;
		zoomLevel += 0.01;// *zoomFactor;
	}
	else if (yoffset < 0)
	{
		// Zoom out
		orthoLeft = worldX + (orthoLeft - worldX) * zoomFactor;
		orthoRight = worldX + (orthoRight - worldX) * zoomFactor;
		orthoBottom = worldY + (orthoBottom - worldY) * zoomFactor;
		orthoTop = worldY + (orthoTop - worldY) * zoomFactor;
		zoomLevel -= 0.01;// / zoomFactor;
	}

	// After zooming, calculate the new view dimensions
	float newViewWidth = (orthoRight - orthoLeft) / zoomLevel;
	float newViewHeight = (orthoTop - orthoBottom) / zoomLevel;

	// Adjust camera position to keep the worldX, worldY under the cursor stationary
	Position.x = worldX - ndcX * (newViewWidth / 2.0f);
	Position.y = worldY - ndcY * (newViewHeight / 2.0f);

	//if (zoomLevel <= 0.001)
	//	zoomLevel = 0.002;
	//if (zoomLevel >= 10.0f)
	//	zoomLevel = 10.0f;

	// Update orthographic projection settings
	SetOrthographic(orthoLeft, orthoRight, orthoBottom, orthoTop, nearPlane, farPlane);

}

// Mouse button callback to handle middle mouse button press
void Camera::MouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
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

// Cursor position callback to handle panning
void Camera::CursorPositionCallback(GLFWwindow* window, double xpos, double ypos) {
	Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
	if (cam->middleMousePressed) {
		glm::vec2 currentMousePosition(xpos, ypos);
		glm::vec2 delta = currentMousePosition - cam->lastMousePosition;

		// Get viewport dimensions
		int viewportWidth, viewportHeight;
		glfwGetWindowSize(window, &viewportWidth, &viewportHeight);

		// Calculate world-space delta based on orthographic dimensions and window size
		float orthoWidth = cam->orthoRight - cam->orthoLeft;
		float orthoHeight = cam->orthoTop - cam->orthoBottom;

		// Convert pixel delta to world space delta by dividing by window size, then multiply by ortho width/height
		float deltaX = (delta.x / viewportWidth) * orthoWidth;
		float deltaY = (delta.y / viewportHeight) * orthoHeight;

		// Update the orthographic bounds directly
		cam->orthoLeft -= deltaX;
		cam->orthoRight -= deltaX;
		cam->orthoBottom += deltaY;
		cam->orthoTop += deltaY;

		// Update last mouse position for the next event
		cam->lastMousePosition = currentMousePosition;

		// Update orthographic projection with the new bounds
		cam->SetOrthographic(cam->orthoLeft, cam->orthoRight, cam->orthoBottom, cam->orthoTop, cam->nearPlane, cam->farPlane);
	}
}


