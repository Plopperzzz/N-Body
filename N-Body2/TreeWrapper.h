#ifndef TREEWRAPPER_H
#define TREEWRAPPER_H
#pragma once
#include "Tree.h"
#include "Utils.h"
#include "RenderGroup.h"

#include <unordered_map>
#include <vector>

template <typename VecType>
class TreeWrapper
{
public:
	std::vector<Node<VecType>> nodeList;
	std::unordered_map<BodyType, std::vector<Node<VecType>>> bodiesByType;


private:
	std::shared_ptr<Tree<VecType>> m_tree;
	int m_totalBodies;


public:
	TreeWrapper(std::shared_ptr<Tree<VecType>> root);

	// Getters
	int getTotalBodies();

	Tree<VecType>& getTree();

	Node<VecType>& operator[](std::size_t index);

	// 
	void insertBody(Node<VecType>& body);
	void deleteBody(int id, std::shared_ptr<Tree<VecType>> tree);

	void calculateForce(Node<VecType>& body, Node<VecType>& other);
	void calculateForce(Node<VecType>& body, const VecType position, const double& mass);
	void calculateForceBi(Node<VecType>& body, Node<VecType>& other);

	void updateForce(Node<VecType>& body, std::shared_ptr<Tree<VecType>> tree);
	void update(const double& dt);

	void loadBodies(const std::string& filePath);
	void extractPositions(std::vector<float>& positions);
	void extractPositions(std::unordered_map<BodyType, RenderGroup>& positions);

	void getBoundingBoxVertices(RenderGroup& group);

private:
};

using TreeWrapper3D = TreeWrapper<glm::dvec3>;
using TreeWrapper2D = TreeWrapper<glm::dvec2>;

#include "TreeWrapper.tpp"
#endif

