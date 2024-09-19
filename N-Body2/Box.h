#pragma once
#include <glm/glm.hpp>
#include <sstream>
class Box
{
	/*
				   +--------------+ topRight
				  /|             /|
				 / |            / |  h
				+--------------+  |  e
				|  |           |  |  i
				|  |           |  |  g
				|  |           |  |  h
				|  |           |  |  t
		z  y	|  +-----------|--+  bottomRight
		| /	 	| /            | /
		|/		|/    width    |/    length
		+---x	+--------------+
				bottomLeft
	*/

	// Members
public:
	glm::dvec3 center;
	glm::dvec3 color;

private:
	double m_halfLength;
	double m_halfWidth;
	double m_halfHeight;

	// Methods
public:
	Box(glm::dvec3 center, const double& halfLength, const double& halfWidth, const double& halfHeight);
	Box(glm::dvec3 center, const double& halfLength, const double& halfWidth, const double& halfHeight, glm::dvec3 color);

	// Getters
	double getLength();
	double getHalfLength();
	double getWidth();
	double getHalfWidth();
	double getHeight();
	double getHalfHeight();

	// Setters
	void setLength(double& length);
	void setWidth(double& width);
	void setHeight(double& height);

	bool virtual isPointContained(glm::dvec3 point);
	bool virtual isInstersecting(Box box);
	bool virtual isContained(Box box);

	void scaleX(double& xScale);
	void scaleY(double& yScale);
	void scaleZ(double& zScale);

	std::stringstream getPositionAsString();
};


//class Rectangle : public Box
//{
//private:
//	double _length;
//	double _width;
//
//public:
//	Rectangle(glm::dvec3 bLeft, glm::dvec3 tRight);
//	Rectangle(glm::dvec3 bLeft, glm::dvec3 tRight, glm::dvec4 color);
//
//	// Getters
//	double getLength();
//	double getWidth();
//
//	// General methods
//	bool isPointContained(glm::dvec3 point);
//	bool isInstersecting(Rectangle box);
//	bool isContained(Rectangle box);
//};
