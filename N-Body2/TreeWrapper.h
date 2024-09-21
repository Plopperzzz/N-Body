#pragma once
#include "Tree.h"
#include "Utils.h"

class TreeWrapper
{
public:
	std::vector<Node> nodeList;

private:
	std::shared_ptr<OctTree> m_tree;
	int m_totalBodies;


public:
	TreeWrapper(std::shared_ptr<OctTree> root);

	// Getters
	int getTotalBodies();

	OctTree& getTree();

	Node& operator[](std::size_t index);

	void insertBody(Node& body);

	void calculateForce(Node& body, const Node& other);
	void calculateForce(Node& body, const glm::dvec3 position, const double& mass);

	void updateForce(Node& body, std::shared_ptr<OctTree> tree);
	void update(const double& dt);

	void loadBodies(std::string filePath);
};

