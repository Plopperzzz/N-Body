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
#include "GFX.h"
#include <EBO.h>
#include <VBO.h>
#include <VAO.h>


#ifdef _DEBUG
std::string path = "./OpenGL";
#else
std::string path = "../../N-Body2/OpenGL";
#endif


unsigned int width = 1200;
unsigned int height = 1200;


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
	// Get the current working directory
	std::filesystem::path cwd = std::filesystem::current_path();

	// Print the current working directory
	std::cout << "Current Working Directory: " << cwd << std::endl;
	// Compile vertex shader
	Shader star_shader((path + "/default.vert").c_str(), (path + "/star.frag").c_str());
	Shader blackhole_shader((path + "/default.vert").c_str(), (path + "/BlackHole.frag").c_str());
	Shader planet_shader((path + "/default.vert").c_str(), (path + "/Planet.frag").c_str());
	Shader wormhole_shader((path + "/default.vert").c_str(), (path + "/WormHole.frag").c_str());
	Shader default_shader((path + "/default.vert").c_str(), (path + "/default.frag").c_str());

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
	//std::vector<float> positions;
	std::unordered_map<BodyType, RenderGroup> positions;
	//extractPositions(TestTree2d, positions);
	TestTree2d.extractPositions(positions);
	GLsizei stride = (2 + 4 + 1) * sizeof(float); // x, y, r, g, b, a, Radius

	for (auto& [type, renderGroup] : positions)
	{
		switch (type)
		{
		case BodyType::Planet:
			renderGroup.Init(planet_shader, stride);
			break;
		case BodyType::Star:
			renderGroup.Init(star_shader, stride);
			break;
		case BodyType::Blackhole:
			renderGroup.Init(blackhole_shader, stride);
			break;
		case BodyType::Wormhole:
			renderGroup.Init(wormhole_shader, stride);
			break;
		default:
			renderGroup.Init(default_shader, stride);
			break;
		}
	}

	// After updating rootLength
	rootLength = TestTree2d.getTree().getLength();

	//	// Set up projection matrix for orthographic projection
	float left = -rootLength / 2;
	float right = rootLength / 2;
	float bottom = -rootLength / 2;
	float top = rootLength / 2;
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

		GFXFrameTime(lastTime, nbFrames);
		// Update simulation
		TestTree2d.update(dt);

		TestTree2d.extractPositions(positions);
			glClearColor(BLACK, 1.0f); // Dark gray background
			//glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // Dark gray background
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		for (auto& [type, renderGroup] : positions)
		{
			renderGroup.Render(camera, maxRad, 7, NULL);
		}

		// Swap buffers and poll IO events
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	for (auto& [type, renderGroup] : positions)
	{
		// Deallocate resources
		renderGroup.vao.Delete();
		renderGroup.vbos[0].Delete();
		renderGroup.vbos[1].Delete();


	}
	// Terminate GLFW
	glfwTerminate();

	/*************************************************************/
	/************************ END SETUP **************************/
	/*************************************************************/
	signal(SIGINT, Utils::signalHandler);

}
