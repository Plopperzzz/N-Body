#pragma once
#include "Tree.h"
#include "Utils.h"

class TreeWrapper
{
public:
	std::vector<std::shared_ptr<Node>> nodeList;

private:
	std::shared_ptr<OctTree> m_tree;
	int m_totalBodies;


public:
	TreeWrapper(std::shared_ptr<OctTree> root);

	// Getters
	int getTotalBodies();

	OctTree& getTree();

	std::shared_ptr<Node> operator[](std::size_t index);

	void insertBody(std::shared_ptr<Node> body);

	void calculateForce(std::shared_ptr<Node> body, std::shared_ptr<Node> other);
	void calculateForce(std::shared_ptr<Node> body, const glm::dvec3 position, const double& mass);

	void updateForce(std::shared_ptr<Node> body, std::shared_ptr<OctTree> tree);
	void update(const double& dt);

	void loadBodies(std::string filePath);
};

