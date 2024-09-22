#pragma once
#include "Tree.h"
#include "Utils.h"

class TreeWrapper
{
public:
	std::vector<Node3D> nodeList;

private:
	std::shared_ptr<OctTree> m_tree;
	int m_totalBodies;


public:
	TreeWrapper(std::shared_ptr<OctTree> root);

	// Getters
	int getTotalBodies();

	OctTree& getTree();

	Node3D& operator[](std::size_t index);

	void insertBody(Node3D& body);

	void calculateForce(Node3D& body, const Node3D& other);
	void calculateForce(Node3D& body, const glm::dvec3 position, const double& mass);

	void updateForce(Node3D& body, std::shared_ptr<OctTree> tree);
	void update(const double& dt);

	void loadBodies(std::string filePath);
};

