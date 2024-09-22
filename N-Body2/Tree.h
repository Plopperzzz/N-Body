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
class QuadTreeWrapper;

class OctTree : public std::enable_shared_from_this<OctTree>
{
	friend class TreeWrapper;

private:
	Node3D m_body;

	Box3D m_boundingBox;

	std::array<std::shared_ptr<OctTree>, PARTITIONS> m_children;

	glm::dvec3 m_centerOfMass;

	static const glm::dvec3 basis[8];

	double m_totalMass;
	static double m_theta;
 	static double m_epsilon;

	int m_totalDescendants;
	
public:
	// Members
	enum Octant {
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
	OctTree(Box3D boundingBox);
	OctTree(Box3D boundingBox, Node3D& body);
	OctTree(Box3D boundingBox, double& theta, double& epsilon);

	// Returns QuadTree child at childIndex
	std::shared_ptr<OctTree>& operator[](std::size_t childIndex);

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
	void setBoundingBoxColor(const glm::dvec4& color);

	// bool
	bool isLeaf();
	bool inBounds(glm::dvec3& position);

	// void
	void subdivide();
	void insertBody(Node3D& body);
	void updateCenterOfMass(Node3D& body);


	// returns the parent container for a point assuming an unbounded box
	// if a Box has center point, < 1, 1 >, then point < 50, 50 > is considerd
	// in that Box even if the Box has a width of 1.
	//
	// Use: to find which portion of a smaller tree a point belongs to in order to 
	// force the tree to grow to contain it.
	Octant findOctant(glm::dvec3& point);
};

