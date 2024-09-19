#include "TreeWrapper.h"
#include "Utils.h"
#include <iostream>
#include <filesystem>

// TODO:
//		- Memorypool class
//		- In-place tree updates
//		- Dynamic time stepping (global or per Node)

int main()
{
	std::string path = "Data\\Earth-Moon.json";
	double dt = 100;
	double rootLength = 5e15;
	double theta = 0.5;
	double epsilon = 1e-9;

	std::cout << "Current working directory: " << std::filesystem::current_path() << std::endl;

	Box bb(glm::dvec3(0.0, 0.0, 0.0), rootLength / 2, rootLength / 2, rootLength / 2);
	std::shared_ptr<OctTree> root = std::make_shared<OctTree>(bb, theta, epsilon);
	TreeWrapper Tree(root);

	Tree.loadBodies(path);

}
