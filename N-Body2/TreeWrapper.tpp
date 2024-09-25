#ifndef TREEWRAPPER_TPP
#define TREEWRAPPER_TPP
#include "TreeWrapper.h"
#include <Windows.h>

template <typename VecType>
TreeWrapper<VecType>::TreeWrapper(std::shared_ptr<Tree<VecType>> root) :
	nodeList(),
	m_totalBodies(0),
	m_tree(root)
{
}

template <typename VecType>
Node<VecType>& TreeWrapper<VecType>::operator[](std::size_t index)
{
	return nodeList[index];
}

template <typename VecType>
int TreeWrapper<VecType>::getTotalBodies()
{
	return m_totalBodies;
}

template <typename VecType>
Tree<VecType>& TreeWrapper<VecType>::getTree()
{
	return *m_tree;
}

template <typename VecType>
void TreeWrapper<VecType>::insertBody(Node<VecType>& body)
{
	// TODO: grow to adapt to new nodes
	// Use the Tree insertion function
	m_tree->insertBody(body);


	// Create a copy
	Node<VecType> nodeCopy = body;
	nodeList.push_back(nodeCopy);
	++m_totalBodies;
}



// Calculates the forces between to Nodes, body and other, and updates `body`s force
template <typename VecType>
void TreeWrapper<VecType>::calculateForce(Node<VecType>& body, const Node<VecType>& other)
{
	VecType distance = body.position - other.position;
	double norm = glm::length(distance);

	if (norm > m_tree->m_epsilon)
	{
		// Should probably add a dampening factor
		body.force += -G * body.mass * other.mass * distance / (norm * norm * norm);
	}
	else
	{
		// to prevent this
		std::cerr << "WARNING: Distance between bodies is too small\n" << "------ " << body.name << std::endl;
	}
}

template<typename VecType>
void TreeWrapper<VecType>::calculateForceBi(Node<VecType>& body, const Node<VecType>& other)
{
	VecType distance = body.position - other.position;
	double norm = glm::length(distance);

	if (norm > m_tree->m_epsilon)
	{
		// Should probably add a dampening factor
		body.force += -G * body.mass * other.mass * distance / (norm * norm * norm);
		other.force += -body.force;
	}
	else
	{
		// to prevent this
		std::cerr << "WARNING: Distance between bodies is too small\n" << "------ " << body.name << std::endl;
	}
}

// Calculates the forces between a body and a point mass, and updates `body`s force
// Used in case we are calculating the force between a Node and a center of mass
template <typename VecType>
void TreeWrapper<VecType>::calculateForce(Node<VecType>& body, const VecType position, const double& mass)
{
	VecType distance = body.position - position;
	double norm = glm::length(distance);

	if (norm > m_tree->m_epsilon)
	{
		body.force += -G * body.mass * mass * distance / (norm * norm * norm);
	}
	else
	{
		std::cerr << "WARNING: Distance between bodies is too small\n" << "------ " << body.name << std::endl;
	}
}

template <typename VecType>
void TreeWrapper<VecType>::updateForce(Node<VecType>& body, std::shared_ptr<Tree<VecType>> tree)
{
	bool leaf = tree->isLeaf();
	bool threshold = (tree->getLength() / glm::length(body.position - tree->m_centerOfMass)) < tree->m_theta;

	// Debug: Print out the key variables to check their values
	DEBUG_LOG("---- %s ----\n", body.name.c_str());
	DEBUG_LOG("%s: Checking threshold\n", __func__);
	DEBUG_LOG("\tTree length: %.2f\n", tree->getLength());
	DEBUG_LOG("\tBody position: <%.2f, %.2f, %.2f>\n", body.position.x, body.position.y, body.position.z);
	DEBUG_LOG("\tCenter of mass: <%.2f, %.2f, %.2f>\n", tree->m_centerOfMass.x, tree->m_centerOfMass.y, tree->m_centerOfMass.z);
	DEBUG_LOG("\tTheta: %.2f\n", tree->m_theta);
	DEBUG_LOG("\tThreshold: %d\n", threshold);

	// Check if it's a leaf, empty region, or if it is the body we are already updating
	if (leaf && (tree->m_body.getId() == -1 || tree->m_body.getId() == body.getId()))
	{
		DEBUG_LOG("%s: Leaf, no valid body or self-body, skipping force update.\n", __func__);

		return;
	}

	// Check if it is a branch
	else if (!leaf) {
		DEBUG_LOG("%s: Branch detected.\n", __func__);


		if (threshold && tree->m_totalDescendants) {

			DEBUG_LOG("*********************************************************************************\n");

			calculateForce(body, tree->m_centerOfMass, tree->m_totalMass);
		}
		else {

			int i = 0;
			for (auto& childTree : tree->m_children) {
				if (childTree->m_totalDescendants > 0)
					updateForce(body, childTree);
				++i;
			}
		}
	}

	// not a branch, leaf, or invalid node (nullptr or `body`)
	else {
		DEBUG_LOG("%s: Leaf or invalid node, calculating force directly.\n", __func__);

		calculateForce(body, tree->m_body);
	}

	return;

}

template <typename VecType>
void TreeWrapper<VecType>::update(const double& dt)
{
	auto total_time = std::chrono::duration<double>::zero();
	double max = m_tree->m_boundingBox.getHalfLength();
	double max_test;

	bool expand = false;

	for (Node<VecType>& body : nodeList) {
		// This should never happen, but hey.
		if (body.getId() == -1) {
			std::cout << "found null body in update loop\n";
		}

		/** Velocity verlet integration **/

		// Calculate acceleration from force and get the new position
		VecType acc = body.force / body.mass;
		VecType new_pos = body.position + body.velocity * dt + acc * (dt * dt * 0.5);

		// Reset the force
		body.force = VecType(0);

		updateForce(body, m_tree);

		// Getting ready to create a new body to insert into newTree
		VecType new_force = body.force;
		VecType new_accel = new_force / body.mass;
		VecType new_vel = body.velocity + (acc + new_accel) * (dt * 0.5);

		body.position = new_pos;
		body.velocity = new_vel;

		// Keep track of the furthest body from the center of the tree to determine if it needs to grow
		// There may be a better, less expensive way of doing this, but hopefully iterating through nodelist again

		// and rebuilding at most once per update outweighs potentially rebuilding multiple times per update and not
		// iterating through the list a second time
		max_test = glm::length(new_pos);
		if (max_test > max) {
			//std::cout << "max_test: " << max_test << std::endl;
			max = max_test;
			expand = true;
		}
	}

	if (expand)
	{
		max *= 2;
	}

	// Create new tree so we dont move bodies before all forces are calcualted
	Box<VecType> newBoundingBox = Box<VecType>(m_tree->m_boundingBox.center, max, max, max);
	std::shared_ptr<Tree<VecType>> newTree = std::make_shared<Tree<VecType>>(newBoundingBox);

	// Create new wrapper to utilize its insertion that will expand the member tree if needed

	for (Node<VecType>& body : nodeList) {
		Node<VecType> bodyCopy = body;
		newTree->insertBody(bodyCopy);
	}

	// Replace the old tree with the new tree
	//m_tree = std::move(newTree);
	m_tree = newTree;
	return;
}

#include <nlohmann/json.hpp>
template <typename VecType>
void TreeWrapper<VecType>::loadBodies(const std::string& file_path) {
	std::ifstream file(file_path);

	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << file_path << std::endl;
		return;
	}

	nlohmann::json body_data = nlohmann::json::parse(file);
	double max = 0;

	for (const auto& body_json : body_data["bodies"])
	{
		double current_distance;
		if constexpr (VecDimensions<VecType>::value == 3)
		{
			current_distance = glm::length(VecType(body_json["position"][0], body_json["position"][1], body_json["position"][2]));
		}
		else
		{
			current_distance = glm::length(VecType(body_json["position"][0], body_json["position"][1]));
		}
		if (current_distance > max)
		{
			max = current_distance;
		}
	}

	Box<VecType> new_bounding_box(m_tree->m_boundingBox.center, 2 * max, 2 * max, 2 * max);

	std::shared_ptr<Tree<VecType>> new_tree = std::make_shared<Tree<VecType>>(new_bounding_box);
	m_tree = new_tree;

	// Loop through each body in the JSON data
	for (const auto& body_json : body_data["bodies"]) {

		int l_id = body_json["id"];
		double l_mass = body_json["mass"];
		double l_radius = body_json["radius"];
		std::string l_name = body_json["name"];

		VecType pos, vel;

		if constexpr (VecDimensions<VecType>::value == 3)
		{
			pos = VecType(body_json["position"][0], body_json["position"][1], body_json["position"][2]);
			vel = VecType(body_json["velocity"][0], body_json["velocity"][1], body_json["velocity"][2]);
		}
		else
		{
			pos = VecType(body_json["position"][0], body_json["position"][1]);
			vel = VecType(body_json["velocity"][0], body_json["velocity"][1]);
		}

		Node<VecType> body = Node<VecType>(
			l_id, l_name,
			pos, vel,
			l_mass, l_radius);

		insertBody(body);
	}

	std::cout << "Finished loding bodies from " << file_path << std::endl;

	return;
}

#endif


