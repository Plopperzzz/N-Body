#pragma once
#include <glm/glm.hpp>
#include <type_traits>
#include <sstream>
#include <vector>

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
    // Single constructor with conditional logic using if constexpr (C++17)
    Box():
        center(VecType()),
        color(1.0),
        m_halfLength(),
        m_halfWidth(),
        m_halfHeight()
    {
        if constexpr (std::is_same_v<VecType, glm::dvec2>) {
            // For 2D, ignore halfHeight or set it to a default value
            m_halfHeight = 0.0;
        }
    }
    // Single constructor with conditional logic using if constexpr (C++17)
    Box(const VecType& center_, double halfLength, double halfWidth, double halfHeight = 0.0):
        center(center_),
        color(1.0),
        m_halfLength(halfLength),
        m_halfWidth(halfWidth),
        m_halfHeight(halfHeight)
    {
        if constexpr (std::is_same_v<VecType, glm::dvec2>) {
            // For 2D, ignore halfHeight or set it to a default value
            m_halfHeight = 0.0;
        }
    }
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

    void getCorners(std::vector<float>& cornerVector)
    {
        return getCorners_impl(cornerVector, std::is_same <VecType, glm::dvec3>());
    }

    void incrementColor() {
        float inc = 0.25;
        color.r -= inc; // Increment red component
        if (color.r < 0.0f) {
            color.r = 1.0f;
            color.g -= inc; // Increment green component when red wraps around
            if (color.g < 0.0f) {
                color.g = 1.0f;
                color.b -= inc; // Increment blue component when green wraps around
                if (color.b < 0.0f) {
                    color.b = 1.0f; // Reset blue if it overflows
                }
            }
        }
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

    // Implementation for 2D
    void getCorners_impl(std::vector<float>& cornerVector, std::false_type)
    {
        glm::dvec2 point = center;
        double halfLen = m_halfLength;

        // Bottom-left corner
        cornerVector.push_back(center.x - halfLen);  // x coordinate
        cornerVector.push_back(center.y - halfLen);  // y coordinate
        cornerVector.push_back(color.r);
        cornerVector.push_back(color.g);
        cornerVector.push_back(color.b);

        // Bottom-right corner
        cornerVector.push_back(center.x + halfLen);  // x coordinate
        cornerVector.push_back(center.y - halfLen);  // y coordinate
        cornerVector.push_back(color.r);
        cornerVector.push_back(color.g);
        cornerVector.push_back(color.b);

        // Top-right corner
        cornerVector.push_back(center.x + halfLen);  // x coordinate
        cornerVector.push_back(center.y + halfLen);  // y coordinate
        cornerVector.push_back(color.r);
        cornerVector.push_back(color.g);
        cornerVector.push_back(color.b);

        // Top-left corner
        cornerVector.push_back(center.x - halfLen);  // x coordinate
        cornerVector.push_back(center.y + halfLen);  // y coordinate
        cornerVector.push_back(color.r);
        cornerVector.push_back(color.g);
        cornerVector.push_back(color.b);

    }
    
    // Implementation for 3D
    void getCorners_impl(std::vector<float>& cornerVector, std::true_type)
    {
        glm::dvec3 point = center;  // Center of the cube
        double halfLen = m_halfLength;  // Half the side length (distance from center to edge)

        // Bottom face (starting from bottom-front-left)

        //    // Loop over all combinations of -halfLen and +halfLen for x, y, and z
        //for (int i = 0; i < 8; ++i)
        //{
        //    // Calculate the sign for each dimension based on the loop index (i)
        //    float xOffset = (i & 1) ? halfLen : -halfLen;
        //    float yOffset = (i & 2) ? halfLen : -halfLen;
        //    float zOffset = (i & 4) ? halfLen : -halfLen;

        //    // Push the corner into the vector
        //    cornerVector.push_back(center.x + xOffset);
        //    cornerVector.push_back(center.y + yOffset);
        //    cornerVector.push_back(center.z + zOffset);
        //}

        // Bottom-front-left
        cornerVector.push_back(center.x - halfLen);
        cornerVector.push_back(center.y - halfLen);
        cornerVector.push_back(center.z - halfLen);

        // Bottom-front-right
        cornerVector.push_back(center.x + halfLen);
        cornerVector.push_back(center.y - halfLen);
        cornerVector.push_back(center.z - halfLen);

        // Bottom-back-right
        cornerVector.push_back(center.x + halfLen);
        cornerVector.push_back(center.y - halfLen);
        cornerVector.push_back(center.z + halfLen);

        // Bottom-back-left
        cornerVector.push_back(center.x - halfLen);
        cornerVector.push_back(center.y - halfLen);
        cornerVector.push_back(center.z + halfLen);

        // Top face (starting from top-front-left)

        // Top-front-left
        cornerVector.push_back(center.x - halfLen);
        cornerVector.push_back(center.y + halfLen);
        cornerVector.push_back(center.z - halfLen);

        // Top-front-right
        cornerVector.push_back(center.x + halfLen);
        cornerVector.push_back(center.y + halfLen);
        cornerVector.push_back(center.z - halfLen);

        // Top-back-right
        cornerVector.push_back(center.x + halfLen);
        cornerVector.push_back(center.y + halfLen);
        cornerVector.push_back(center.z + halfLen);

        // Top-back-left
        cornerVector.push_back(center.x - halfLen);
        cornerVector.push_back(center.y + halfLen);
        cornerVector.push_back(center.z + halfLen);
    }
};

// Type aliases for convenience
using Box2D = Box<glm::dvec2>;
using Box3D = Box<glm::dvec3>;
