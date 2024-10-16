#pragma once
#include <glad/glad.h>

#include "Renderer.h"
#include "VBO.h"

class VAO
{
public:
	GLuint ID;
	VAO();
	void Delete();

	void LinkAttrib(VBO& vbo, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset);
	void Bind();
	void Unbind();
};

