#include "TreeWrapper.h"
#include <nlohmann/json.hpp>
#include <Windows.h>

TreeWrapper::TreeWrapper(std::shared_ptr<OctTree> root) :
	nodeList(),
	m_totalBodies(0),
	m_tree(root)
{
}

std::shared_ptr<Node> TreeWrapper::operator[](std::size_t index)
{
	return nodeList[index];
}

int TreeWrapper::getTotalBodies()
{
	return m_totalBodies;
}

OctTree& TreeWrapper::getTree()
{
	return *m_tree;
}

void TreeWrapper::insertBody(std::shared_ptr<Node> body)
{
	// TODO: grow to adapt to new nodes
	// Use the Tree insertion function
	m_tree->insertBody(body);

	// Create a copy
	Node nodeCopy = *body;
	nodeList.push_back(std::make_shared<Node>(nodeCopy));
	++m_totalBodies;
}


// Calculates the forces between to Nodes, body and other, and updates `body`s force
void TreeWrapper::calculateForce(std::shared_ptr<Node> body, std::shared_ptr<Node> other)
{
	glm::dvec3 distance = body->position - other->position;

	double norm = glm::length(distance);
	DEBUG_LOG("%s: norm: %.2f\n", __func__, norm);

	DEBUG_LOG("%s--%s:\n\t\tPre-calculation Force: <%.2f, %.2f, %.2f>\n", __func__, body->name.c_str(),
		body->force.x, body->force.y, body->force.z);

	if (norm > m_tree->m_epsilon)
	{
		// Should probably add a dampening factor
		body->force += -G * body->mass * other->mass * distance / (norm * norm * norm);
	}
	else
	{
		// to prevent this
		std::cerr << "WARNING: Distance between bodies is too small\n" << "------ " << body->name << std::endl;
	}
	DEBUG_LOG("%s: Other position: <%.2f, %.2f, %.2f>\n", __func__, other->position.x, other->position.y, other->position.z);
	DEBUG_LOG("%s (PM) : Other mass: %.2f\n", __func__, other->mass);
	DEBUG_LOG("%s--%s:\n\t\tPost-calculation Force: <%.2f, %.2f, %.2f>\n", __func__, body->name.c_str(),
		body->force.x, body->force.y, body->force.z);
}

// Calculates the forces between a body and a point mass, and updates `body`s force
// Used in case we are calculating the force between a Node and a center of mass
void TreeWrapper::calculateForce(std::shared_ptr<Node> body, const glm::dvec3 position, const double& mass)
{
	glm::dvec3 distance = body->position - position;

	double norm = glm::length(distance);
	DEBUG_LOG("%s: norm: %.2f\n", __func__, norm);

	if (norm > m_tree->m_epsilon)
	{
		body->force += -G * body->mass * mass * distance / (norm * norm * norm);
	}
	else
	{
		std::cerr << "WARNING: Distance between bodies is too small\n" << "------ " << body->name << std::endl;
	}		
	DEBUG_LOG("%s: Other position: <%.2f, %.2f, %.2f>\n", __func__, position.x, position.y, position.z);
	DEBUG_LOG("%s (PM) : Other mass: %.2f\n", __func__, mass);
	DEBUG_LOG("%s--%s:\n\t\tPost-calculation Force: <%.2f, %.2f, %.2f>\n", __func__, body->name.c_str(),
		body->force.x, body->force.y, body->force.z);
}

void TreeWrapper::updateForce(std::shared_ptr<Node> body, std::shared_ptr<OctTree> tree)
{
	bool leaf =tree->isLeaf();
	bool threshold = (tree->getLength() / (body->position - tree->m_centerOfMass).length()) < tree->m_theta;

	// Debug: Print out the key variables to check their values
	DEBUG_LOG("%s: Checking threshold\n", __func__);
	DEBUG_LOG("\tTree length: %.2f\n", tree->getLength());
	DEBUG_LOG("\tBody position: <%.2f, %.2f, %.2f>\n", body->position.x, body->position.y, body->position.z);
	DEBUG_LOG("\tCenter of mass: <%.2f, %.2f, %.2f>\n", tree->m_centerOfMass.x, tree->m_centerOfMass.y, tree->m_centerOfMass.z);
	DEBUG_LOG("\tTheta: %.2f\n", tree->m_theta);
	DEBUG_LOG("\tThreshold: %d\n", threshold);

	// Check if it's a leaf, empty region, or if it is the body we are already updating
	if (leaf && (tree->m_body == nullptr || tree->m_body->getId() == body->getId()))
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

			for (int i = 0; i < PARTITIONS; ++i) {
				if((*tree)[i]->m_totalDescendants > 0)
					updateForce(body, (*tree)[i]);
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

void TreeWrapper::update(const double& dt)
{
	auto total_time = std::chrono::duration<double>::zero();
	double max = m_tree->m_boundingBox.getHalfLength();
	double max_test;

	bool expand = false;

	for (std::shared_ptr<Node> body : nodeList) {
		// This should never happen, but hey.
		if (body == nullptr) {
			std::cout << "found null body in update loop\n";
		}

		/** Velocity verlet integration **/

		// Calculate acceleration from force and get the new position
		glm::dvec3 acc = body->force / body->mass;
		glm::dvec3 new_pos = body->position + body->velocity * dt + acc * (dt * dt * 0.5);

		// Reset the force
		body->force = glm::dvec3(0);

		updateForce(body, m_tree);

		// Getting ready to create a new body to insert into newTree
		glm::dvec3 new_force = body->force;
		glm::dvec3 new_accel = new_force / body->mass;
		glm::dvec3 new_vel = body->velocity + (acc + new_accel) * (dt * 0.5);

		(*body).position = new_pos;
		(*body).velocity = new_vel;

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
	Box newBoundingBox = Box(m_tree->m_boundingBox.center, max, max, max);
	std::shared_ptr<OctTree> newTree = std::make_shared<OctTree>(newBoundingBox);

	// Create new wrapper to utilize its insertion that will expand the member tree if needed

	for (std::shared_ptr<Node> body : nodeList) {
		std::shared_ptr<Node> bodyCopy = std::make_shared<Node>(*body);
		newTree->insertBody(bodyCopy);
	}

	// Replace the old tree with the new tree
	//m_tree = std::move(newTree);
	m_tree = newTree;

	return;
}


using json = nlohmann::json;

void TreeWrapper::loadBodies(std::string file_path) {
	std::ifstream file(file_path);

	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << file_path << std::endl;
		return;
	}

	json body_data = json::parse(file);
	double max = 0;
	double norm;


	// Loop through each body in the JSON data
	for (const auto& body_json : body_data["bodies"]) {

		int l_id = body_json["id"];
		double l_mass = body_json["mass"];
		double l_radius = body_json["radius"];
		std::string l_name = body_json["name"];

		glm::dvec3 pos(body_json["position"][0], body_json["position"][1], body_json["position"][2]);

		std::shared_ptr<Node> body = std::make_shared<Node>(
			l_id, l_name,
			glm::dvec3(body_json["position"][0], body_json["position"][1], body_json["position"][2]),
			glm::dvec3(body_json["velocity"][0], body_json["velocity"][1], body_json["velocity"][2]),
			l_mass, l_radius);

		insertBody(body);
	}

	std::cout << "Finished loding bodies from " << file_path << std::endl;

	return;
}



