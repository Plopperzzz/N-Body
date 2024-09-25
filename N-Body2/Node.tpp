#ifndef NODE_TPP
#define NODE_TPP
#include "Node.h"
#include <fstream>


// Node
template <typename VecType>
Node<VecType>::Node() :
	m_id(-1),
	name(),
	position(),
	velocity(),
	force(),
	mass(0.0),
	radius(0.0)
{}

template <typename VecType>
Node<VecType>::Node(int id, std::string name, VecType position, VecType velocity, double const mass, double const radius) :
	m_id(id),
	position(position),
	velocity(velocity),
	force(VecType(0)),
	name(name),
	mass(mass),
	radius(radius)
{}

template <typename VecType>
int Node<VecType>::getId() const{
	return m_id;
}

template <typename VecType>
void Node<VecType>::setId(int id) {
	m_id = id;
}

template <typename VecType>
void Node<VecType>::OutputPositionToStream(std::ofstream& file) {
	int vec_length = VecType::length();
	for (int i = 0; i < vec_length; ++i) {
		file << "," << position[i]; // Ensure no leading comma
	}
}

using Node3D = Node<glm::dvec3>;
using Node2D = Node<glm::dvec2>;

#endif NODE_TPP
