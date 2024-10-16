#include "Shader.h"

std::string GetFileContents(const char* filename)
{
	std::ifstream in(filename, std::ios::binary);
	if (in)
	{
		std::string contents;
		in.seekg(0, std::ios::end);
		contents.resize(in.tellg());
		in.seekg(0, std::ios::beg);
		in.read(&contents[0], contents.size());
		in.close();
		return(contents);
	}
	else if (!in.is_open()) {
		std::cerr << "Error: Could not open file: " << filename << std::endl;
		throw std::runtime_error("Failed to open file");
	}
	throw(errno);
}


void CheckShaderCompileErrors(GLuint shader, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER-COMPILATION ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "| ERROR::PROGRAM-LINKING ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

Shader::Shader(const char* vertexFile, const char* fragmentFile)
{
	// Read vertexFile and fragmentFile and store the strings
	std::string vertexCode = GetFileContents(vertexFile);
	std::string fragmentCode = GetFileContents(fragmentFile);

	const char* vertexSource = vertexCode.c_str();
	const char* fragmentSource = fragmentCode.c_str();

	// Compile Vertex Shader
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader, 1, &vertexSource, NULL);
	glCompileShader(vertexShader);
	CheckShaderCompileErrors(vertexShader, "VERTEX");

	// Compile Fragment Shader
	GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
	glCompileShader(fragmentShader);
	CheckShaderCompileErrors(fragmentShader, "FRAGMENT");

	// Shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertexShader);
	glAttachShader(ID, fragmentShader);
	glLinkProgram(ID);
	CheckShaderCompileErrors(ID, "PROGRAM");

	// Delete the shaders as they’re linked into our program now and no longer necessary
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}


void Shader::SetUniform1f(const char* uniform, float v0)
{
	glUniform1f(glGetUniformLocation(ID, uniform), v0);
}

void Shader::Bind()
{
	GLCall(glUseProgram(ID));
}

void Shader::Unbind()
{
	GLCall(glDeleteProgram(ID));
}
