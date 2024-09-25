#define NOMINMAX

#include <cxxopts.hpp>
#include <filesystem>
#include <Windows.h>
#include <iostream>
#include <csignal>

#include "TreeWrapper.h"
#include "Utils.h"

// TODO:
//		- Memorypool class
//		- In-place tree updates
//		- Dynamic time stepping (global or per Node)

int main(int argc, char** argv)
{
	//std::string path = "../../N-Body2/Data/Earth-Moon.json";
	//double dt = 100;
	//double rootLength = 2e9;
	//double theta = 0;
	//double epsilon = 1e-9;


	//Box bb(glm::dvec3(0.0, 0.0, 0.0), rootLength / 2, rootLength / 2, rootLength / 2);
	//std::shared_ptr<OctTree> root = std::make_shared<OctTree>(bb, theta, epsilon);
	//TreeWrapper Tree(root);

	//Tree.loadBodies(path);
	//double nt = 0;
	//root->setTheta(nt);

	//Tree.update(dt);

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
	//std::cout << gif_title.str() << std::endl;

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

	std::ofstream orbitFile(data_name, std::ios::out | std::ios::trunc);
	orbitFile.close();

	orbitFile.open(data_name, std::ios::app);

	auto total_time = std::chrono::duration<double>::zero();
	std::chrono::duration<double> previous_time;

	/*************************************************************/
	/************************ OpenGL SETUP ***********************/
	/*************************************************************/

	/*************************************************************/
	/************************ END SETUP **************************/
	/*************************************************************/


	/*************************************************************/
	/************************** SETUP ****************************/
	/*************************************************************/

	double epsilon = 1e-3;
	double rootLength = 1e5;

	if (!twoD) {
		Box3D bb(glm::dvec3(0.0), rootLength / 2, rootLength / 2, rootLength / 2);
		std::shared_ptr<Tree3D> root = std::make_shared<Tree3D>(bb, theta, epsilon);
		root->setTheta(theta);
		TreeWrapper3D TestTree(root);

		TestTree.loadBodies(input_path);
		rootLength = TestTree.getTree().getLength();



		/*************************************************************/
		/************************ END SETUP **************************/
		/*************************************************************/
		signal(SIGINT, Utils::signalHandler);

		int divFactor = (int)log2(num) + 1;
		for (int i = 0; i < num; ++i) {

			previous_time = total_time;
			total_time += Utils::measureInvokeCall(&TreeWrapper3D::update, TestTree, dt);
			if (plot)
				Utils::outputPositions(TestTree.nodeList, i * dt, orbitFile);

			if (i % divFactor == 0 || i == num)
				Utils::printProgressBar(i, num, 80, "time - " + std::to_string((total_time - previous_time).count()));
		}
		std::cout << std::endl;
		std::cout << "Update -- Average update time for - " << TestTree.getTotalBodies() << " - bodies: " << std::setprecision(15) << total_time.count() / num << std::endl;
		orbitFile.close();

		if (result.count("plot")) {
			std::vector<std::string> node_names;
			for (auto& body : TestTree.nodeList) {
				node_names.push_back(body.name);
			}

			std::cout << script_path << " written\n";
			Utils::gpScript3d(script_path, gif_path, data_name, TestTree.getTree().getLength() / 3, node_names);
			std::cout << "Calling " << script_path << "\n";
			system(gnuCommand.c_str());
			std::cout << "opening " << gif_path << "\n";
			system(gifCommand.c_str());
		}
	}
	else {
		Box2D bb(glm::dvec2(0.0), rootLength / 2, rootLength / 2, rootLength / 2);
		std::shared_ptr<Tree2D> root2d = std::make_shared<Tree2D>(bb, theta, epsilon);
		root2d->setTheta(theta);
		TreeWrapper2D TestTree2d(root2d);

		TestTree2d.loadBodies(input_path);
		rootLength = TestTree2d.getTree().getLength();



		/*************************************************************/
		/************************ END SETUP **************************/
		/*************************************************************/
		signal(SIGINT, Utils::signalHandler);

		int divFactor = (int)log2(num) + 1;
		for (int i = 0; i < num; ++i) {

			previous_time = total_time;
			total_time += Utils::measureInvokeCall(&TreeWrapper2D::update, TestTree2d, dt);
			if (plot)
				Utils::outputPositions(TestTree2d.nodeList, i * dt, orbitFile);

			if (i % divFactor == 0 || i == num)
				Utils::printPrograssBar(i, num, 80, "time - " + std::to_string((total_time - previous_time).count()));
		}
		std::cout << std::endl;
		std::cout << "Update -- Average update time for - " << TestTree2d.getTotalBodies() << " - bodies: " << std::setprecision(15) << total_time.count() / num << std::endl;
		orbitFile.close();

		if (result.count("plot")) {
			std::vector<std::string> node_names;
			for (auto& body : TestTree2d.nodeList) {
				node_names.push_back(body.name);
			}

			std::cout << script_path << " written\n";
			Utils::gpScript(script_path, gif_path, data_name, TestTree2d.getTree().getLength() / 3, node_names);
			std::cout << "Calling " << script_path << "\n";
			system(gnuCommand.c_str());
			std::cout << "opening " << gif_path << "\n";
			system(gifCommand.c_str());
		}
	}
}
