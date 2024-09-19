#include "TreeWrapper.h"
#include <nlohmann/json.hpp>

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
	m_tree->insertBody(body);
	Node nodeCopy = *body;
	nodeList.push_back(std::make_shared<Node>(nodeCopy));
	++m_totalBodies;
}


void TreeWrapper::calculateForce(std::shared_ptr<Node> body, std::shared_ptr<Node> other)
{
	glm::dvec3 distance = body->position - other->position;

	double norm = distance.length();

	if (norm > m_tree->m_epsilon)
	{
		body->force += -G * body->mass * other->mass * distance / (norm * norm * norm);
	}
	else
	{
		std::cerr << "WARNING: Distance between bodies is too small\n" << "------ " << body->name << std::endl;
	}
}

void TreeWrapper::calculateForce(std::shared_ptr<Node> body, const glm::dvec3 position, const double& mass)
{
	glm::dvec3 distance = body->position - position;

	double norm = distance.length();

	if (norm > m_tree->m_epsilon)
	{
		body->force += -G * body->mass * mass * distance / (norm * norm * norm);
	}
	else
	{
		std::cerr << "WARNING: Distance between bodies is too small\n" << "------ " << body->name << std::endl;
	}
}

void TreeWrapper::updateForce(std::shared_ptr<Node> body, std::shared_ptr<OctTree> tree)
{
	bool leaf =tree->isLeaf();
	bool threashold = (tree->getLength() / (body->position - tree->m_centerOfMass).length()) < tree->m_theta;

	if (leaf && (tree->m_body == nullptr || tree->m_body->getId() == body->getId()))
	{
		return;
	}

	else if (!leaf) {

		if (threashold && tree->m_totalDescendants) {
			calculateForce(body, tree->m_centerOfMass, tree->m_totalMass);
		}
		else {
			for (int i = 0; i < PARTITIONS; ++i) {
				updateForce(body, (*tree)[i]);
			}
		}
	}

	else {
		calculateForce(body, tree->m_body->position, tree->m_body->mass);
	}

	return;

}

void update(const double& dt)
{

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

#ifdef _DEBUG
		std::cout << body->name << " path: ";
		for (int& q : *(body->path)) {
			std::cout << q << " ";
		}
		std::cout << std::endl;
#endif

	}
	// print node path for debug 

	std::cout << "Finished loding bodies from " << file_path << std::endl;

	return;
}



