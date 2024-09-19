#include "Box.h"

Box::Box(glm::dvec3 centerPoint,const double& halfLength, const double& halfWidth, const double& halfHeight) :
	center(centerPoint),
	m_halfLength(halfLength),
	m_halfWidth(halfWidth),
	m_halfHeight(halfHeight),
	color(glm::dvec3(0,0,0))
{};

Box::Box(glm::dvec3 center, const double& halfLength, const double& halfWidth, const double& halfHeight, glm::dvec3 lineColor) :
	Box(center, halfLength, halfWidth, halfHeight)
{
	color = lineColor;
}

// Returns length of Box (y-axis)
double Box::getLength() {
	return 2 * m_halfLength;
}

// Returns half the length of Box (y-axis)
double Box::getHalfLength() {
	return m_halfLength;
}

void Box::setLength(double& length)
{
	m_halfLength = length / 2;
}

// Returns width of Box (x-axis)
double Box::getWidth()
{
	return 2 * m_halfWidth;
}

// Returns half the width of Box (x-axis)
double Box::getHalfWidth()
{
	return m_halfWidth;
}

void Box::setWidth(double& width)
{
	m_halfWidth = width / 2;
}

// Returns height of Box (z-axis)
double Box::getHeight() {
	return 2 * m_halfHeight;
}

// Returns half the height of Box (z-axis)
double Box::getHalfHeight()
{
	return m_halfHeight;
}

void Box::setHeight(double& height)
{
	m_halfHeight = height / 2;
}

// Returns true is point is contained within the Box
// Assumes no rotation of the box with respect to global
bool Box::isPointContained(glm::dvec3 point) {
	bool xContained = (point.x >= center.x - m_halfWidth) && (point.x <= center.x + m_halfWidth);
	bool yContained = (point.y >= center.y - m_halfLength) && (point.y <= center.y + m_halfLength);
	bool zContained = (point.z >= center.z - m_halfHeight) && (point.z <= center.z + m_halfHeight);

	return xContained && yContained && zContained;
}

bool Box::isInstersecting(Box box) {
	return true;
}
bool Box::isContained(Box box) {
	return true;
}

void Box::scaleX(double& xScale) {
	m_halfWidth *= xScale;
}

void Box::scaleY(double& yScale) {
	m_halfLength *= yScale;
}

void Box::scaleZ(double& zScale) {
	m_halfHeight *= zScale;
}

std::stringstream Box::getPositionAsString() {
	std::stringstream positionString;

	positionString << "Rect:   < " << center.x << " , " << center.y << " , " << center.z << " >\n"
		"Length: " << getLength() << "\n"
		"Color:  < " << color.x << " , " << color.y << " , " << color.z << " >\n";

	return positionString;
}
