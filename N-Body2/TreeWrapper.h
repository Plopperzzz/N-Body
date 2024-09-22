#ifndef TREEWRAPPER_H
#define TREEWRAPPER_H
#pragma once
#include "Tree.h"
#include "Utils.h"

template <typename VecType>
class TreeWrapper
{
public:
	std::vector<Node3D> nodeList;

private:
	std::shared_ptr<Tree> m_tree;
	int m_totalBodies;


public:
	TreeWrapper(std::shared_ptr<Tree> root);

	// Getters
	int getTotalBodies();

	Tree& getTree();

	Node3D& operator[](std::size_t index);

	void insertBody(Node3D& body);

	void calculateForce(Node3D& body, const Node3D& other);
	void calculateForce(Node3D& body, const glm::dvec3 position, const double& mass);

	void updateForce(Node3D& body, std::shared_ptr<Tree> tree);
	void update(const double& dt);

	void loadBodies(std::string filePath);
};

using TreeWrapper3D = TreeWrapper<glm::dvec3>;

#include "TreeWrapper.tpp"
#endif
