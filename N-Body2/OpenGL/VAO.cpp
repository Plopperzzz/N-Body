#include "VAO.h"

VAO::VAO()
{
	GLCall(glGenVertexArrays(1, &ID));
}

void VAO::Delete()
{
	GLCall(glDeleteVertexArrays(1, &ID));
}

void VAO::LinkAttrib(VBO& VBO, GLuint layout, GLuint numComponents, GLenum type, GLsizeiptr stride, void* offset)
{
	VBO.Bind();
	glVertexAttribPointer(layout, numComponents, type, GL_FALSE, stride, offset);
	glEnableVertexAttribArray(layout);
	VBO.Unbind();
}


void VAO::Bind()
{
	GLCall(glBindVertexArray(ID));
}

void VAO::Unbind()
{
	GLCall(glBindVertexArray(0));
}
