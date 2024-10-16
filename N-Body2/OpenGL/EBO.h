#pragma once
#include <glad/glad.h>
#include "Renderer.h"
class EBO
{
public:
	GLuint ID;
	EBO(GLuint* indices, GLsizeiptr size);
	void Delete();

	void Bind();
	void Unbind();
};

