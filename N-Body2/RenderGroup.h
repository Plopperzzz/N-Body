#pragma once
#include "VBO.h"
#include "VAO.h"
#include "Shader.h"
#include "Camera.h"

#include "Constants.h"

#include <vector>

class RenderGroup
{
public:
	VBO vbos[2];
	VAO vao;
	Shader shader;

	GLsizeiptr stride;
	GLuint currentBuffer = 0;

	std::vector<float> data;

	RenderGroup();

	void Init(Shader shader, GLsizeiptr stride);

	void Render(Camera& cam, float maxRad, GLuint attribCount, GLFWwindow* window);
};

