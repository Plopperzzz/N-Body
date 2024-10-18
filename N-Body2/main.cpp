#define NOMINMAX

#include <cxxopts.hpp>
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <csignal>

#include "TreeWrapper.h"
#include "Utils.h"

#include <glad/glad.h>
#include <Shader.h>
#include <Camera.h>
#include <EBO.h>
#include <VBO.h>
#include <VAO.h>

#define WHITE 1.0,1.0,1.0
#define BLACK 0.0,0.0,0.0
#define DGRAY 0.1,0.2,0.1

unsigned int width = 1200;
unsigned int height = 1200;

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
	glViewport(0, 0, 4000, 4000);
	glfwSwapInterval(0);
	
	return window;

}


// TODO:
//		- Memorypool class
//		- In-place tree updates
//		- Dynamic time stepping (global or per Node)

int zoom = 0;

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
	std::ofstream orbitFile(data_name, std::ios::out | std::ios::trunc);
	orbitFile.close();

	orbitFile.open(data_name, std::ios::app);

	auto total_time = std::chrono::duration<double>::zero();
	std::chrono::duration<double> previous_time;

	GLFWwindow* window = (GLFWwindow*)GFXInit(width, height, 3, 3, "N-Body Simulation");

	glEnable(GL_POINT_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glPointSize(6.0f);
	// Compile vertex shader
	Shader shader("../../N-Body2/OpenGL/default.vert", "../../N-Body2/OpenGL/default.frag");

	/*************************************************************/
	/************************** SETUP ****************************/
	/*************************************************************/

	double epsilon = 1e-4;
	double rootLength = 1e5;

	Box2D bb(glm::dvec2(0.0), rootLength / 2, rootLength / 2, rootLength / 2);
	std::shared_ptr<Tree2D> root2d = std::make_shared<Tree2D>(bb, theta, epsilon);
	root2d->setTheta(theta);
	TreeWrapper2D TestTree2d(root2d);

	TestTree2d.loadBodies(input_path);
	rootLength = TestTree2d.getTree().getLength();

	double maxRad = TestTree2d.nodeList[0].radius;
	for (auto& body : TestTree2d.nodeList)
	{
		if (body.radius > maxRad)
		{
			maxRad = body.radius;
		}
	}


	// Extract positions from the TreeWrapper
	std::vector<float> positions;
	//extractPositions(TestTree2d, positions);
	TestTree2d.extractPositions(positions);

	// Create buffers/arrays
	//GLuint VBOs[2];// , VAO;
	VAO vao;
	vao.Bind();

	VBO vbo[2] = { VBO(positions.data(), positions.size() * sizeof(float)), VBO(positions.data(), positions.size() * sizeof(float)) };

	GLsizei stride = (2 + 4 + 1) * sizeof(float); // x, y, r, g, b, a, Radius

	for (int i = 0; i < 2; ++i) {
		// Position data
		vao.LinkAttrib(vbo[i], 0, 2, GL_FLOAT, stride, (void*)0);

		// Color data
		vao.LinkAttrib(vbo[i], 1, 4, GL_FLOAT, stride, (void*)(2 * sizeof(float)));  // Color

		// Radius data
		vao.LinkAttrib(vbo[i], 2, 1, GL_FLOAT, stride, (void*)(6 * sizeof(float)));  // Color
	}

	vao.Unbind();

	// After updating rootLength
	rootLength = TestTree2d.getTree().getLength();

	// Set up projection matrix for orthographic projection
	float left = -rootLength;// / 2;
	float right = rootLength;// / 2;
	float bottom = -rootLength;// / 2;
	float top = rootLength;// / 2;
	float nearPlane = -1.0;
	float farPlane = 1.0;

	// Set up camera and input callbacks
	Camera camera(width, height, glm::vec3(0.0f, 0.0f, 0.0f));
	camera.SetOrthographic(left, right, bottom, top, nearPlane, farPlane);
	glfwSetWindowUserPointer(window, &camera); // Set user pointer to the camera instance

	glfwSetScrollCallback(window, [](GLFWwindow* window, double xoffset, double yoffset) {
		Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		cam->ScrollCallback(window, xoffset, yoffset);
		});
	glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		if (action == GLFW_PRESS || action == GLFW_REPEAT) {
			cam->KeyCallback(window, key, scancode, action, mods);
		}
		}); // Set the input callback
	glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
		Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		cam->MouseButtonCallback(window, button, action, mods);
		});

	glfwSetCursorPosCallback(window, [](GLFWwindow* window, double xpos, double ypos) {
		Camera* cam = static_cast<Camera*>(glfwGetWindowUserPointer(window));
		cam->CursorPositionCallback(window, xpos, ypos);
		});

	// Variables for FPS calculation
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	// Get the location of the uniform variable "uProjection" in the shader program
	//GLint projLoc = glGetUniformLocation(shader.ID, "camMatrix");

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
		//extractPositions(TestTree2d, positions);
		TestTree2d.extractPositions(positions);

		// Update the vertex buffer
		vbo[currentBuffer].Bind();
		glBufferSubData(GL_ARRAY_BUFFER, 0, positions.size() * sizeof(float), positions.data());
		//glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
		vbo[currentBuffer].Unbind();

		// Switch to the other buffer for the next frame
		currentBuffer = (currentBuffer + 1) % 2;

		// Render
		glClearColor(BLACK, 1.0f); // Dark gray background
		//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Dark gray background
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Use the shader program
		shader.Bind();

		// Set the uniform for the projection matrix
		camera.Matrix(shader, "camMatrix");
		shader.SetUniform1f("radius", maxRad);
		shader.SetUniform1f("zoom", camera.zoomLevel);;

		// Bind VAO
		vao.Bind();


		// Bind the appropriate VBO
		glBindBuffer(GL_ARRAY_BUFFER, vbo[currentBuffer].ID);

		// Draw points
		glDrawArrays(GL_POINTS, 0, positions.size() / 2); // positions.size() / 2 for 2D

		// Unbind VAO
		vao.Unbind();

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Deallocate resources
	vao.Delete();
	vbo[0].Delete();
	vbo[1].Delete();

	// Terminate GLFW
	glfwTerminate();

	/*************************************************************/
	/************************ END SETUP **************************/
	/*************************************************************/
	signal(SIGINT, Utils::signalHandler);

}

