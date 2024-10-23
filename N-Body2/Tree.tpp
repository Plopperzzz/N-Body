#ifndef TREE_TPP
#define TREE_TPP
#include "Tree.h"
#include <iostream>
inline int maxCount = 65;
template <typename VecType>
const glm::dvec3 Tree<VecType>::basis[8] = {
	glm::dvec3(-1, -1, -1), // Index 0: bSOUTHWEST
	glm::dvec3(1, -1, -1),  // Index 1: bSOUTHEAST
	glm::dvec3(-1, 1, -1),  // Index 2: bNORTHWEST
	glm::dvec3(1, 1, -1),   // Index 3: bNORTHEAST
	glm::dvec3(-1, -1, 1),  // Index 4: tSOUTHWEST
	glm::dvec3(1, -1, 1),   // Index 5: tSOUTHEAST
	glm::dvec3(-1, 1, 1),   // Index 6: tNORTHWEST
	glm::dvec3(1, 1, 1)     // Index 7: tNORTHEAST
};

template <typename VecType>
double Tree<VecType>::m_theta = 0.5;
template <typename VecType>
double Tree<VecType>::m_epsilon = 1e-3;

template <typename VecType>
Tree<VecType>::Tree(Box<VecType> boundingBox, Node<VecType>& body) ://, std::weak_ptr<OctTree> parent) :
	m_centerOfMass(VecType(0)),
	m_body(),
	m_boundingBox(boundingBox),
	m_totalDescendants(0),
	m_totalMass(0),
	m_maxBodyCount(maxCount),
	m_currentBodyCount(1)
{
	m_body.push_back(body);
}

template <typename VecType>
Tree<VecType>::Tree(Box<VecType> boundingBox) ://, std::weak_ptr<OctTree> parent) :
	m_totalDescendants(0),
	m_totalMass(0),
	m_boundingBox(boundingBox),
	m_centerOfMass(VecType(0)),
	m_maxBodyCount(maxCount),
	m_currentBodyCount(0)
{
}

template <typename VecType>
Tree<VecType>::Tree(Box<VecType> boundingBox, double& theta, double& epsilon) :
	m_boundingBox(boundingBox),
	m_centerOfMass(VecType(0)),
	m_totalMass(0),
	m_totalDescendants(0),
	m_maxBodyCount(maxCount),
	m_currentBodyCount(0)
{
}

template <typename VecType>
std::shared_ptr<Tree<VecType>>& Tree<VecType>::operator[](std::size_t childIndex) {
	return m_children[childIndex];
}

template <typename VecType>
double Tree<VecType>::getLength() {
	return m_boundingBox.getLength();
}

template <typename VecType>
double Tree<VecType>::getMass() {
	return m_totalMass;
}

template <typename VecType>
double& Tree<VecType>::getTheta() {
	return m_theta;
}

template <typename VecType>
void Tree<VecType>::setTheta(double& theta) {
	m_theta = theta;
}

template <typename VecType>
double& Tree<VecType>::getEpsilon() {
	return m_epsilon;
}

template<typename VecType>
int Tree<VecType>::getCurrentBodyCount() const
{
	return m_currentBodyCount;
}

template<typename VecType>
int Tree<VecType>::getNodeIndex(Node<VecType>& body) const
{
	int i = 0;
	if(m_currentBodyCount)
		for (i; body.getId() != m_body[i].getId(); ++i);
	return i;
}

template <typename VecType>
void Tree<VecType>::setEpsilon(double& epsilon) {
	m_epsilon = epsilon;
}

template <typename VecType>
int Tree<VecType>::getTotalDescendants() {
	return m_totalDescendants;
}

template <typename VecType>
glm::dvec3 Tree<VecType>::getBoundingBoxColor() {
	return m_boundingBox.color;
}

template <typename VecType>
void Tree<VecType>::setBoundingBoxColor(const glm::dvec3& color)
{
	m_boundingBox.color = color;
}

template<typename VecType>
void Tree<VecType>::setMaxBodyCount(size_t newMax)
{
	m_maxBodyCount = newMax;
}

template <typename VecType>
bool Tree<VecType>::isLeaf() {
	for (auto& child : m_children)
	{
		if (child)
			return false;
	}
	return true;
}

template <typename VecType>
void Tree<VecType>::subdivide()
{
	double halfLength = m_boundingBox.getHalfLength() / 2;

	// retrieve once and use in loop
	VecType thisCenter = m_boundingBox.center;

	int i = 0;
	for (auto& child : m_children)
	{
		child = std::make_shared<Tree<VecType>>(
			Box<VecType>(
				thisCenter + halfLength * (VecType(basis[i])),
				halfLength,
				halfLength,
				halfLength));
		child->m_boundingBox.color = m_boundingBox.color;
		child->m_boundingBox.incrementColor();
		++i;
	}
}

template <typename VecType>
void Tree<VecType>::updateCenterOfMass(Node<VecType>& body) {
	double mass = body.mass;
	m_centerOfMass = (m_centerOfMass * m_totalMass + mass * body.position) / (m_totalMass + mass);
	m_totalMass += mass;
}


template <typename VecType>
bool Tree<VecType>::inBounds(VecType& position) {
	return m_boundingBox.contains(position);
}

template <typename VecType>
typename Tree<VecType>::Region Tree<VecType>::findRegion(VecType& point) {

	VecType center = m_boundingBox.center;
	int index = 0;

	if (point.x > center.x) index |= 1;  // 1 if east, 0 if west
	if (point.y > center.y) index |= 2;  // 1 if north, 0 if south

	if constexpr (VecDimensions<VecType>::value == 3) {
		if (point.z > center.z) index |= 4;  // 1 if top, 0 if bottom
	}

	// Lookup table for quadrants
	switch (index) {
	case 0: return Tree::bSOUTHWEST;  // (west, south, bottom)
	case 1: return Tree::bSOUTHEAST;  // (east, south, bottom)
	case 2: return Tree::bNORTHWEST;  // (west, north, bottom)
	case 3: return Tree::bNORTHEAST;  // (east, north, bottom)
	case 4: return Tree::tSOUTHWEST;  // (west, south, top)
	case 5: return Tree::tSOUTHEAST;  // (east, south, top)
	case 6: return Tree::tNORTHWEST;  // (west, north, top)
	case 7: return Tree::tNORTHEAST;  // (east, north, top)
	default: return Tree::QVOID;  // Should never happen
	}
}

template <typename VecType>
void Tree<VecType>::insertBody(Node<VecType>& body)
{
	if (inBounds(body.position) == false)
	{
		return;
	}
	Tree<VecType>::Region region, currentInhabitantNewRegion;

	++m_totalDescendants;
	updateCenterOfMass(body);

	region = findRegion(body.position);

	if (m_currentBodyCount < m_maxBodyCount && isLeaf()) {

		// it is a leaf, and empty, so we can insert the node here
		m_body.push_back(body);
		m_currentBodyCount++;
		//body->path->push_back(octant);
		return;

	}


	//body->path->push_back(octant);

	if (m_currentBodyCount == 0) {

		// it is not a leaf, so we need to recurse.
		m_children[region]->insertBody(body);

	}

	else {
		// it is a leaf (it contains the maximum number of bodies and no children), so
		// subdivide and we need to re-insert the body that previously
		// populated the current quad
		subdivide();

		for (auto currentBody : m_body)
		{

			Node<VecType> currentInhabitant = currentBody;

			currentInhabitantNewRegion = findRegion(currentInhabitant.position);

			m_children[currentInhabitantNewRegion]->insertBody(currentInhabitant);
		}
		m_body.resize(0);
		m_currentBodyCount = 0;
		m_children[region]->insertBody(body);
	}

	return;
}

#endif
