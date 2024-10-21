#pragma once

#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cerrno>

#include "Renderer.h"

std::string GetFileContents(const char* filename);
void CheckShaderCompileErrors(GLuint shader, std::string type);

class Shader
{
public:
	GLuint ID;
	Shader() :ID() {};
	Shader(const char* vertexFile, const char* fragmentFile);


	void SetUniform1f(const char* uniform, float v0);
	void Bind();
	void Unbind();
};

