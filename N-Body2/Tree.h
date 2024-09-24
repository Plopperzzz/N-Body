#ifndef TREE_H
#define TREE_H
#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include <array>

#include "constants.h"
#include "BoxBase.h"
#include "Node.h"

/*********************

class/struct:
	ClassName

public:
	m_publicVariableName

private:
	_privateVariableName

methods:
	getPrivateMember()
	setPrivateMember()

*********************/

/*
                  +--------------+--------------+
                 /|     6-tNW   /|    7-tNE    /|
                / |            / |            / |
			   +--------------+--------------+  |
			  /|  |          /|  |          /|  |
			 / |  |  4-tSW  / |  |  5-SE   / |  |
			+--------------+--------------+  |  |
			|  |  |        |  |  |        |  |  |
	    	|  |  +-----------|--+-----------|--+
	   	 	|  | /|        |  | /|        |  | /|
  z	  		|  |/ |        |  |/ |        |  |/ |
  |	 y		|  +-----------|--+-----------|--+  |
  |	/       | /|  |        | /|  |        | /|  |
  |/        |/ |  |        |/ |  |        |/ |  |
  +----x    +--------------+--------------+  |  |
			|  |  |        |  |  |        |  |  |
			|  |  +--------|--|--+--------|--|--+
			|  | /   2-bNW |  | /  3-bNE  |  | /
			|  |/          |  |/          |  |/
	    	|  +-----------|--+-----------|--+
	   	 	| /            | /            | /
	  		|/   0-bSW     |/   1-bSE     |/ 
	     	+--------------+--------------+

*/
// Primary template: Not defined to trigger a compile-time error for unsupported types
template <typename VecType>
struct TreePartitions;

// Specialization for glm::dvec2
template <>
struct TreePartitions<glm::dvec2> {
	static constexpr size_t value = 4;
};

// Specialization for glm::dvec3
template <>
struct TreePartitions<glm::dvec3> {
	static constexpr size_t value = 8;
};
template <typename VecType>
class TreeWrapper;

template <typename VecType>
class Tree
{
	template <typename VecType>
	friend class TreeWrapper;

private:

	// Actual data
	Node<VecType> m_body;

	// Region defining the Tree
	Box<VecType> m_boundingBox;

	// Children: total number based on glm::dvec2 (4) or glm::dvec3 (8)
	static constexpr size_t partitions = TreePartitions<VecType>::value;
	std::array<std::shared_ptr<Tree<VecType>>, partitions> m_children;

	VecType m_centerOfMass;

	// Array of unit directions to aid finding / creating regions
	static const glm::dvec3 basis[8];

	double m_totalMass;

	// Barnes-Hut threshold
	static double m_theta;

	// Error threshold
 	static double m_epsilon;

	// Total number of Nodes contained in all the leaves
	int m_totalDescendants;
	
public:
	// Members
	enum Region {
		bSOUTHWEST = 0,
		bSOUTHEAST = 1,
		bNORTHWEST = 2,
		bNORTHEAST = 3,
		tSOUTHWEST = 4,
		tSOUTHEAST = 5,
		tNORTHWEST = 6,
		tNORTHEAST = 7,
		QVOID = -1
	};

	// Methods
public:
	Tree<VecType>(Box<VecType> boundingBox);
	Tree<VecType>(Box<VecType> boundingBox, Node<VecType>& body);
	Tree<VecType>(Box<VecType> boundingBox, double& theta, double& epsilon);

	// Returns QuadTree child at childIndex
	std::shared_ptr<Tree<VecType>>& operator[](std::size_t childIndex);

	// Getters
	double getLength();
	double getMass();
	double& getTheta();
	double& getEpsilon();

	int getTotalDescendats();

	glm::dvec3 getBoundinBoxColor();

	// Setters
	void setTheta(double& newTheta);
	void setEpsilon(double& newEpsilon);
	void setBoundingBoxColor(const glm::dvec3& color);

	// bool
	bool isLeaf();
	bool inBounds(VecType& position);

	// void
	void subdivide();
	void insertBody(Node<VecType>& body);
	void updateCenterOfMass(Node<VecType>& body);


	// returns the parent container for a point assuming an unbounded box
	// if a Box has center point, < 1, 1 >, then point < 50, 50 > is considerd
	// in that Box even if the Box has a width of 1.
	//
	// Use: to find which portion of a smaller tree a point belongs to in order to 
	// force the tree to grow to contain it.
	Region findRegion(VecType& point);
};
using Tree2D = Tree<glm::dvec2>;
using Tree3D = Tree<glm::dvec3>;
#include "Tree.tpp"
#endif

