#pragma once
#ifndef NODE_H
#define NODE_H

#include <glm/glm.hpp>
#include <iostream>
#include <fstream>
#include <string>

#include "constants.h"
#include "BoxBase.h"

/*********************
/*********************

class/struct:
	ClassName

public:
	publicVariableName

private:
	m_privateVariableName

function variables:
	function_variable

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
template <typename VecType>
struct Node {
private:
	int m_id;

public:
	std::string name;
	VecType position;
	VecType velocity;
	VecType force;
	glm::vec4 color;
	double mass;
	double radius;


public:

	// Default constructor
	Node();

	// Creates new node with given properties
	Node(int id, std::string name, VecType position, VecType velocity, double const mass, double const radius, glm::vec4 color);

	// Gets node ID
	int getId() const;
	void setId(int id);

	// Various methods
	void OutputPositionToStream(std::ofstream& file);
};

// Helper to determine vector dimensions
template <typename VecType>
struct VecDimensions;

template <glm::length_t L, typename T, glm::precision P>
struct VecDimensions<glm::vec<L, T, P>> {
	static constexpr size_t value = L;
};

#include "Node.tpp"

#endif
