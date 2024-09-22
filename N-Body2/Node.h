#pragma once
#include <glm/glm.hpp>
#include <memory>
#include <string>
#include <vector>
#include <array>

#include "constants.h"
#include "BoxBase.h"

/*********************
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

struct Node {
private:
	int m_id;

public:
	std::string name;
	glm::dvec3 position;
	glm::dvec3 velocity;
	glm::dvec3 force;
	//std::shared_ptr<std::vector<int>> path;
	double mass;
	double radius;


public:

	// Default constructor
	Node();

	// Creates new node with given properties
	Node(int id, std::string name, glm::dvec3 position, glm::dvec3 velocity, double const mass, double const radius);

	// Gets node ID
	int getId();
	void setId(int id);

};

