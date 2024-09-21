#pragma once
#include <glm/glm.hpp>
#include <type_traits>
#include <sstream>

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
// Generic Box Template
template <typename VecType>
class Box {
    static_assert(std::is_same<VecType, glm::dvec2>::value ||
        std::is_same<VecType, glm::dvec3>::value,
        "Box can only be instantiated with glm::dvec2 or glm::dvec3");

public:
    VecType center;
    glm::dvec3 color; // Keeping color as 3D (RGB)

private:
    double m_halfLength;
    double m_halfWidth;

    // For 3D, include halfHeight conditionally
    double m_halfHeight;

public:
    // Constructor for 2D
    template <typename T = VecType>
    Box(const VecType& center_, const glm::dvec3& color_,
        double halfLength, double halfWidth,
        typename std::enable_if<std::is_same<T, glm::dvec2>::value, double>::type = 0)
        : center(center_), color(color_),
        m_halfLength(halfLength), m_halfWidth(halfWidth), m_halfHeight(0.0) {}

    // Constructor for 3D
    template <typename T = VecType>
    Box(const VecType& center_, const glm::dvec3& color_,
        double halfLength, double halfWidth, double halfHeight,
        typename std::enable_if<std::is_same<T, glm::dvec3>::value, double>::type = 0)
        : center(center_), color(color_),
        m_halfLength(halfLength), m_halfWidth(halfWidth), m_halfHeight(halfHeight) {}

    template <typename T = VecType>
    Box(const VecType& center_,
        double halfLength, double halfWidth, double halfHeight,
        typename std::enable_if<std::is_same<T, glm::dvec3>::value, double>::type = 0)
        : center(center_), color(glm::dvec3(0)),
        m_halfLength(halfLength), m_halfWidth(halfWidth), m_halfHeight(halfHeight) {}

    // Methods

    // --- Length (Y-axis) Methods ---
    // Returns length of Box (y-axis)
    double getLength() const {
        return 2.0 * m_halfLength;
    }

    // Returns half the length of Box (y-axis)
    double getHalfLength() const {
        return m_halfLength;
    }

    // Sets the length of Box (y-axis)
    void setLength(const double& length) {
        m_halfLength = length / 2.0;
    }

    // --- Width (X-axis) Methods ---
    // Returns width of Box (x-axis)
    double getWidth() const {
        return 2.0 * m_halfWidth;
    }

    // Returns half the width of Box (x-axis)
    double getHalfWidth() const {
        return m_halfWidth;
    }

    // Sets the width of Box (x-axis)
    void setWidth(const double& width) {
        m_halfWidth = width / 2.0;
    }

    // --- Height (Z-axis) Methods ---
    // These methods are only available for 3D Boxes

    // Returns height of Box (z-axis)
    template <typename T = VecType>
    typename std::enable_if<std::is_same<T, glm::dvec3>::value, double>::type
        getHeight() const {
        return 2.0 * m_halfHeight;
    }

    // Returns half the height of Box (z-axis)
    template <typename T = VecType>
    typename std::enable_if<std::is_same<T, glm::dvec3>::value, double>::type
        getHalfHeight() const {
        return m_halfHeight;
    }

    // Sets the height of Box (z-axis)
    template <typename T = VecType>
    typename std::enable_if<std::is_same<T, glm::dvec3>::value, void>::type
        setHeight(const double& height) {
        m_halfHeight = height / 2.0;
    }

    // --- Additional Methods ---

    // Example method to check if a point is inside the box
    // Overloaded for 2D and 3D
    bool contains(const VecType& point) const {
        return contains_impl(point, std::is_same<VecType, glm::dvec3>());
    }

private:
    // Implementation for 2D
    bool contains_impl(const glm::dvec2& point, std::false_type) const {
        return (point.x >= center.x - m_halfWidth &&
            point.x <= center.x + m_halfWidth &&
            point.y >= center.y - m_halfLength &&
            point.y <= center.y + m_halfLength);
    }

    // Implementation for 3D
    bool contains_impl(const glm::dvec3& point, std::true_type) const {
        return (point.x >= center.x - m_halfWidth &&
            point.x <= center.x + m_halfWidth &&
            point.y >= center.y - m_halfLength &&
            point.y <= center.y + m_halfLength &&
            point.z >= center.z - m_halfHeight &&
            point.z <= center.z + m_halfHeight);
    }
};

// Type aliases for convenience
using Box2D = Box<glm::dvec2>;
using Box3D = Box<glm::dvec3>;
