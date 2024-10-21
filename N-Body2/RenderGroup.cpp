#include "RenderGroup.h"

RenderGroup::RenderGroup()
{
}

void RenderGroup::Init(Shader shader, GLsizeiptr stride)
{
	this->shader = shader;
	this->stride = stride;

	vao.Bind();
	for (int i = 0; i < 2; ++i)
	{
		vbos[i] = VBO(data.data(), data.size() * sizeof(float));

		// Position data
		vao.LinkAttrib(vbos[i], 0, 2, GL_FLOAT, stride, (void*)0);

		// Color data
		vao.LinkAttrib(vbos[i], 1, 4, GL_FLOAT, stride, (void*)(2 * sizeof(float)));  // Color

		// Radius data
		vao.LinkAttrib(vbos[i], 2, 1, GL_FLOAT, stride, (void*)(6 * sizeof(float)));  // Color
	}

	vao.Unbind();
}

void RenderGroup::Render(Camera& cam, float maxRad, GLuint attribCount, GLFWwindow* window = NULL)
{
			// Update the vertex buffer
			vbos[currentBuffer].Bind();
			glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
			//glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
			vbos[currentBuffer].Unbind();

			// Switch to the other buffer for the next frame
			currentBuffer = (currentBuffer + 1) % 2;

			// Use the shader program
			shader.Bind();

			// Set the uniform for the projection matrix
			cam.Matrix(shader, "camMatrix");
			shader.SetUniform1f("radius", maxRad);
			shader.SetUniform1f("zoom", cam.zoomLevel);;

			// Bind VAO
			vao.Bind();


			// Bind the appropriate VBO
			glBindBuffer(GL_ARRAY_BUFFER, vbos[currentBuffer].ID);

			// Draw points
			glDrawArrays(GL_POINTS, 0, data.size() / attribCount); // positions.size() / 2 for 2D

			// Unbind VAO
			vao.Unbind();
}
