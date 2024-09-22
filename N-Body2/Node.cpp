#include "Node.h"

// Node
Node::Node():
	m_id(-1)
{}

Node::Node(int id, std::string name, glm::dvec3 position, glm::dvec3 velocity, double const mass, double const radius) :
	m_id(id),
	position(position),
	velocity(velocity),
	force(glm::dvec3(0)),
	name(name),
	mass(mass),
	radius(radius)
	//path(std::make_shared<std::vector<int>>())
{}

int Node::getId() {
	return m_id;
}

void Node::setId(int id) {
	m_id = id;
}
