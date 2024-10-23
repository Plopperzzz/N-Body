#include "GFX.h"

void* GFXInit(unsigned int width, unsigned int height, unsigned char major, unsigned char minor, const char* windowTitle)
{
	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return 0;
	}


	// Set OpenGL version (3.3 Core)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, major);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, minor);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// For Mac OS X
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle, NULL, NULL);
	if (!window) {
		std::cerr << "Failed to open GLFW window\n";
		glfwTerminate();
		return 0;
	}
	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLEW (note: must be done after making context current)
	gladLoadGL();
	// After initializing GLEW and before your render loop
	GLCall(glEnable(GL_PROGRAM_POINT_SIZE));

	// Viewport
	glViewport(0, 0, 1920, 1920);
	glfwSwapInterval(0);

	return window;

}

void GFXFrameTime(double& lastTime, int& nbFrames)
{
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last print was more than 1 sec ago
			// Calculate FPS and frame time
			double fps = double(nbFrames) / (currentTime - lastTime);
			double msPerFrame = 1000.0 / fps;

			// Print to console
			std::cout << msPerFrame << " ms/frame (" << fps << " FPS)" << std::endl;

			// Reset for next calculation
			nbFrames = 0;
			lastTime = currentTime;
		}
}

