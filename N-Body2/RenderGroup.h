#pragma once
#include "VBO.h"
#include "VAO.h"
#include "EBO.h"
#include "Shader.h"
#include "Camera.h"

#include "Constants.h"

#include <vector>

class RenderGroup
{
public:
	VBO vbos[2];
	VAO vao;
	EBO ebos[2];
	Shader shader;

	GLsizeiptr stride;
	GLuint currentBuffer = 0;

	std::vector<float> data;
	std::vector<GLuint> indices;  // Optional indices array

	RenderGroup();
	~RenderGroup();

	void Init(Shader shader, GLsizeiptr stride);

	void Render(Camera& cam, float maxRad, GLuint attribCount, GLFWwindow* window);
};

