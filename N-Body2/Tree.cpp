#include "Tree.h"
#include <iostream>


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

const glm::dvec3 OctTree::basis[8] = {
	glm::dvec3(-1, -1, -1), // Index 0: bSOUTHWEST
	glm::dvec3(1, -1, -1),  // Index 1: bSOUTHEAST
	glm::dvec3(-1, 1, -1),  // Index 2: bNORTHWEST
	glm::dvec3(1, 1, -1),   // Index 3: bNORTHEAST
	glm::dvec3(-1, -1, 1),  // Index 4: tSOUTHWEST
	glm::dvec3(1, -1, 1),   // Index 5: tSOUTHEAST
	glm::dvec3(-1, 1, 1),   // Index 6: tNORTHWEST
	glm::dvec3(1, 1, 1)     // Index 7: tNORTHEAST
};

double OctTree::m_theta = 0.5;
double OctTree::m_epsilon = 1e-3;

OctTree::OctTree(Box boundingBox, Node& body)://, std::weak_ptr<OctTree> parent) :
	m_centerOfMass(glm::dvec3(0)),
	m_boundingBox(boundingBox),
	m_body(body),
	m_totalDescendants(0),
	m_totalMass(0)
{
}

OctTree::OctTree(Box boundingBox)://, std::weak_ptr<OctTree> parent) :
	m_totalDescendants(0),
	m_totalMass(0),
	m_boundingBox(boundingBox),
	m_centerOfMass(glm::dvec3(0))
{
}
OctTree::OctTree(Box boundingBox, double& theta, double& epsilon) :
	m_boundingBox(boundingBox),
	m_centerOfMass(glm::dvec3(0)),
	m_totalMass(0),
	m_totalDescendants(0)
{
}

std::shared_ptr<OctTree>& OctTree::operator[](std::size_t childIndex) {
	return m_children[childIndex];
}

double OctTree::getLength() {
	return m_boundingBox.getLength();
}

double OctTree::getMass() {
	return m_totalMass;
}

double& OctTree::getTheta() {
	return m_theta;
}

void OctTree::setTheta(double& theta) {
	m_theta = theta;
}

double& OctTree::getEpsilon() {
	return m_epsilon;
}

void OctTree::setEpsilon(double& epsilon) {
	m_epsilon = epsilon;
}

int OctTree::getTotalDescendats() {
	return m_totalDescendants;
}

glm::dvec3 OctTree::getBoundinBoxColor() {
	return m_boundingBox.color;
}

void OctTree::setBoundingBoxColor(const glm::dvec4& color)
{
	m_boundingBox.color = color;
}

bool OctTree::isLeaf() {
	for (auto& child : m_children)
	{
		if (child)
			return false;
	}
	return true;
}

void OctTree::subdivide()
{
	double halfLength = m_boundingBox.getHalfLength() / 2;

	// retrieve once and use in loop
	glm::dvec3 thisCenter = m_boundingBox.center;

	int i = 0;
	for (auto& child : m_children)
	{
		child = std::make_shared<OctTree>(
			Box(
				thisCenter + halfLength * basis[i],
				halfLength,
				halfLength,
				halfLength));
		++i;
	}
}

void OctTree::updateCenterOfMass(Node& body) {
	double mass = body.mass;
	m_centerOfMass = (m_centerOfMass * m_totalMass + mass * body.position) / (m_totalMass + mass);
	m_totalMass += mass;
}

bool OctTree::inBounds(glm::dvec3& position) {
	return m_boundingBox.isPointContained(position);
}

OctTree::Octant OctTree::findOctant(glm::dvec3& point) {

	glm::dvec3 center = m_boundingBox.center;
	int index = 0;

	if (point.x > center.x) index |= 1;  // 1 if east, 0 if west
	if (point.y > center.y) index |= 2;  // 1 if north, 0 if south
	if (point.z > center.z) index |= 4;  // 1 if top, 0 if bottom

	// Lookup table for quadrants
	switch (index) {
	case 0: return OctTree::bSOUTHWEST;  // (west, south, bottom)
	case 1: return OctTree::bSOUTHEAST;  // (east, south, bottom)
	case 2: return OctTree::bNORTHWEST;  // (west, north, bottom)
	case 3: return OctTree::bNORTHEAST;  // (east, north, bottom)
	case 4: return OctTree::tSOUTHWEST;  // (west, south, top)
	case 5: return OctTree::tSOUTHEAST;  // (east, south, top)
	case 6: return OctTree::tNORTHWEST;  // (west, north, top)
	case 7: return OctTree::tNORTHEAST;  // (east, north, top)
	default: return OctTree::QVOID;  // Should never happen
	}
}

void OctTree::insertBody(Node& body)
{
	if (inBounds(body.position) == false)
	{
		return;
	}
	OctTree::Octant octant, currentInhabitantNewQuadrant;

	++m_totalDescendants;
	updateCenterOfMass(body);

	octant = findOctant(body.position);

	if (m_body.getId() == -1 && isLeaf()) {

		// it is a leaf, and empty, so we can insert the node here
		m_body = body;
		//body->path->push_back(octant);
		return;

	}


	//body->path->push_back(octant);

	if (m_body.getId() == -1) {

		// it is not a leaf, so we need to recurse.
		m_children[octant]->insertBody(body);

	}

	else {
		// it is a leaf (it contains one body and no children), so
		// subdivide and we need to re-insert the body that previously
		// populated the current quad

		Node currentInhabitant = m_body;
		m_body = Node();

		currentInhabitantNewQuadrant = findOctant(currentInhabitant.position);

		subdivide();

		m_children[currentInhabitantNewQuadrant]->insertBody(currentInhabitant);
		m_children[octant]->insertBody(body);
	}

	return;
}
