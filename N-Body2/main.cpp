#define NOMINMAX

#include <cxxopts.hpp>
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <csignal>

#include "TreeWrapper.h"
#include "Utils.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

	GLuint shaderProgram;
	glm::mat4 projection;

// Function to extract positions from the TreeWrapper
template <typename VecType>
void extractPositions(const TreeWrapper<VecType>& treeWrapper, std::vector<float>& positions) {
	for (const auto& node : treeWrapper.nodeList) {
		if constexpr (std::is_same_v<VecType, glm::dvec2>) {
			positions.push_back(static_cast<float>(node.position.x));
			positions.push_back(static_cast<float>(node.position.y));
		}
		else if constexpr (std::is_same_v<VecType, glm::dvec3>) {
			positions.push_back(static_cast<float>(node.position.x));
			positions.push_back(static_cast<float>(node.position.y));
			positions.push_back(static_cast<float>(node.position.z));
		}
	}
}

// TODO:
//		- Memorypool class
//		- In-place tree updates
//		- Dynamic time stepping (global or per Node)

// Framebuffer size callback function
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	// Make sure the viewport matches the new window dimensions
	glViewport(0, 0, width, height);

	// Update the projection matrix to maintain the square aspect ratio
	float aspectRatio = (float)width / (float)height;
	float rootLength = 1e5;  // This should match your initial root length

	if (aspectRatio > 1.0f) {
		// Width is larger than height, adjust horizontally
		projection = glm::ortho(-rootLength / 3.0f * aspectRatio, rootLength / 3.0f * aspectRatio,
			-rootLength / 3.0f, rootLength / 3.0f,
			-1.0f, 1.0f);
	}
	else {
		// Height is larger than width, adjust vertically
		projection = glm::ortho(-rootLength / 3.0f, rootLength / 3.0f,
			-rootLength / 3.0f / aspectRatio, rootLength / 3.0f / aspectRatio,
			-1.0f, 1.0f);
	}

	// Get the location of the uniform variable "uProjection" in the shader program
	GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);
}

int main(int argc, char** argv)
{
#ifndef cxx_arguments
	cxxopts::Options options("N-Body", "Simple Barnes-Hut n-body simulation.");

	options.add_options()
		("i,iterations", "Number of iterations to render", cxxopts::value<int>()->default_value("100"))
		("d,delta", "Timestep", cxxopts::value<double>()->default_value("1.0"))
		("h,help", "Print usage")
		("o,out", "Output data file", cxxopts::value<std::string>()->default_value("orbits.csv"))
		("p,plot", "Enable gnuplot plotting", cxxopts::value<bool>()->default_value("false"))
		("s,script", "Gnuplot script file", cxxopts::value<std::string>()->default_value("plot.gp"))
		("g,gif", "GIF output filename", cxxopts::value<std::string>()->default_value("orbits"))
		("theta", "Theta threshold", cxxopts::value<double>()->default_value("0.5"))
		("f,file", "Input point data file (JSON)", cxxopts::value<std::string>()->default_value("../Data/test_bodies-1.json"))
		("b,brute-force", "N-body simulation algorithm", cxxopts::value<bool>()->default_value("false"))
		("twoD", "Choice of 2 or 3", cxxopts::value<bool>()->default_value("false"))
		;

	system("CLS");
	std::stringstream gif_title;
	for (int i = 1; i < argc; ++i) {
		gif_title << argv[i] << "__";
	}

	std::cout << std::endl;

	auto result = options.parse(argc, argv);
	bool plot = false;
	bool brute_force = result["brute-force"].as<bool>();
	bool twoD = result["twoD"].as<bool>();

	if (result.count("brute-force") && result.count("theta"))
	{
		std::cout << "WARNING: --theta option ignored when -b is specified.\n";
		brute_force = true;
	}


	if (result.count("help"))
	{
		std::cout << options.help() << "\n";
		return EXIT_FAILURE;
	}

	int num = result["iterations"].as<int>();
	if (result.count("plot")) {
		plot = result["plot"].as<bool>();
	}

	double dt = result["delta"].as<double>();
	double theta = result["theta"].as<double>();

	std::string input_path = result["file"].as<std::string>();
	std::string data_name = result["out"].as<std::string>() + ".csv";
	std::string gif_path = result["gif"].as<std::string>();
	std::string gifCommand = "powershell.exe Start-Process '" + gif_path + ".gif'";
	std::string script_path = result["script"].as<std::string>();
	std::string gnuCommand = "wsl gnuplot " + script_path;
#endif


	// Global or static variables
double zoomLevel = 1.0;
glm::dvec2 panOffset = glm::dvec2(0.0);

bool middleMousePressed = false;
double lastX = 0.0, lastY = 0.0;
	std::ofstream orbitFile(data_name, std::ios::out | std::ios::trunc);
	orbitFile.close();

	orbitFile.open(data_name, std::ios::app);

	auto total_time = std::chrono::duration<double>::zero();
	std::chrono::duration<double> previous_time;

	/*************************************************************/
	/************************ OpenGL SETUP ***********************/
	/*************************************************************/

	// Initialize GLFW
	if (!glfwInit()) {
		std::cerr << "Failed to initialize GLFW\n";
		return -1;
	}

	// After initializing GLEW and before your render loop
	glEnable(GL_PROGRAM_POINT_SIZE);

	// Set OpenGL version (3.3 Core)
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	// For Mac OS X
	// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

	// Create a windowed mode window and its OpenGL context
	GLFWwindow* window = glfwCreateWindow(800, 800, "Tree Visualization", NULL, NULL);
	if (!window) {
		std::cerr << "Failed to open GLFW window\n";
		glfwTerminate();
		return -1;
	}
	// Make the window's context current
	glfwMakeContextCurrent(window);

	// Initialize GLEW (note: must be done after making context current)
	glewExperimental = GL_TRUE; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW\n";
		glfwTerminate();
		return -1;
	}

	// Viewport
	glViewport(0, 0, 800, 600);

	// Vertex shader source code
	const char* vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos; // For 2D positions
    uniform mat4 uProjection;
    void main() {
        gl_Position = uProjection * vec4(aPos, 0.0, 1.0);
        gl_PointSize = 5.0; // Set point size
    }
    )";

	// Fragment shader source code
	const char* fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(1.0); // White color
    }
    )";

	// Compile vertex shader
	GLuint vertexShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
	glCompileShader(vertexShader);
	// Check for shader compile errors
	GLint success;
	GLchar infoLog[512];
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
		std::cerr << "Vertex Shader Compilation Failed:\n" << infoLog << "\n";
	}

	// Compile fragment shader
	GLuint fragmentShader;
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
	glCompileShader(fragmentShader);
	// Check for shader compile errors
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
		std::cerr << "Fragment Shader Compilation Failed:\n" << infoLog << "\n";
	}

	// Link shaders into a shader program
	GLuint shaderProgram;
	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);
	// Check for linking errors
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
		std::cerr << "Shader Program Linking Failed:\n" << infoLog << "\n";
	}
	// Clean up shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);


	/*************************************************************/
	/************************ END SETUP **************************/
	/*************************************************************/


	/*************************************************************/
	/************************** SETUP ****************************/
	/*************************************************************/

	double epsilon = 1e-3;
	double rootLength = 1e5;

		Box2D bb(glm::dvec2(0.0), rootLength / 2, rootLength / 2, rootLength / 2);
		std::shared_ptr<Tree2D> root2d = std::make_shared<Tree2D>(bb, theta, epsilon);
		root2d->setTheta(theta);
		TreeWrapper2D TestTree2d(root2d);

		TestTree2d.loadBodies(input_path);
		rootLength = TestTree2d.getTree().getLength();


		// Extract positions from the TreeWrapper
		std::vector<float> positions;
		extractPositions(TestTree2d, positions);

		// Create buffers/arrays
		GLuint VBOs[2], VAO;
		glGenVertexArrays(1, &VAO);
		glGenBuffers(2, VBOs);

		// Bind the Vertex Array Object first
		glBindVertexArray(VAO);

		GLsizei stride = 3 * sizeof(float); // x, y, radius for 2D

		for (int i = 0; i < 2; ++i) {
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[i]);
			glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), nullptr, GL_DYNAMIC_DRAW); // Allocate memory

			// Position attribute
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, stride, (void*)0); // x and y
			glEnableVertexAttribArray(0);

			// Radius attribute
			glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, stride, (void*)(2 * sizeof(float))); // radius
			glEnableVertexAttribArray(1);
		}

		// Set the vertex attributes pointers
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0); // For 2D positions
		glEnableVertexAttribArray(0);

		// Unbind the VAO (it's always a good thing to unbind any buffer/array to prevent strange bugs)
		glBindVertexArray(0);
		// After updating rootLength
		rootLength = TestTree2d.getTree().getLength();

		// Set up projection matrix for orthographic projection
		projection = glm::ortho((float) ( - 1 * rootLength/3), (float)rootLength/3,   // Left, Right
			(float) ( - 1 * rootLength/3), (float)rootLength/3,   // Bottom, Top
			-1.0f, 1.0f);                          // Near, Far


		// Variables for FPS calculation
		double lastTime = glfwGetTime();
		int nbFrames = 0;

		// Get the location of the uniform variable "uProjection" in the shader program
		GLint projLoc = glGetUniformLocation(shaderProgram, "uProjection");
		int currentBuffer = 0; // Index to alternate between VBOs
		// Main loop
		while (!glfwWindowShouldClose(window)) {
			// Input handling here (if needed)
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
			// Update simulation
			TestTree2d.update(dt);

			// Extract updated positions
			positions.clear();
			extractPositions(TestTree2d, positions);

			// Update the vertex buffer
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[currentBuffer]);
			glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), positions.data());

			// Switch to the other buffer for the next frame
			currentBuffer = (currentBuffer + 1) % 2;

			// Render
			glClearColor(0.1f, 0.1f, 0.1f, 1.0f); // Dark gray background
			glClear(GL_COLOR_BUFFER_BIT);

			// Use the shader program
			glUseProgram(shaderProgram);
			// Set the uniform for the projection matrix
			glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

			// Bind VAO
			glBindVertexArray(VAO);
			// Bind the appropriate VBO
			glBindBuffer(GL_ARRAY_BUFFER, VBOs[currentBuffer]);
			// Draw points
			glDrawArrays(GL_POINTS, 0, positions.size() / 2); // positions.size() / 2 for 2D
			// Unbind VAO
			glBindVertexArray(0);

			// Swap buffers and poll IO events
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		// Deallocate resources
		glDeleteVertexArrays(1, &VAO);
		glDeleteBuffers(2, VBOs);

		// Terminate GLFW
		glfwTerminate();

		/*************************************************************/
		/************************ END SETUP **************************/
		/*************************************************************/
		signal(SIGINT, Utils::signalHandler);

}



