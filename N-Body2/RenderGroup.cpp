#include "RenderGroup.h"

RenderGroup::RenderGroup()
{
}

RenderGroup::~RenderGroup()
{
	for (int i = 0; i < 2; ++i)
	{
		vbos[i].Delete();
		ebos[i].Delete();
	}
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
		vao.LinkAttrib(vbos[i], 1, 4, GL_FLOAT, stride, (void*)(2 * sizeof(float)));

		// Radius data
		vao.LinkAttrib(vbos[i], 2, 1, GL_FLOAT, stride, (void*)(6 * sizeof(float)));
		if (!this->indices.empty())
		{
			ebos[i] = EBO(this->indices.data(), this->indices.size() * sizeof(float));
		}
	}

	vao.Unbind();
}

void RenderGroup::Render(Camera& cam, float maxRad, GLuint attribCount, GLFWwindow* window = NULL)
{
			//// Update the vertex buffer
			//vbos[currentBuffer].Bind();
			//glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
			////glBufferData(GL_ARRAY_BUFFER, positions.size() * sizeof(float), positions.data(), GL_DYNAMIC_DRAW);
			//vbos[currentBuffer].Unbind();

			//// Switch to the other buffer for the next frame
			//currentBuffer = (currentBuffer + 1) % 2;

			//// Use the shader program
			//shader.Bind();

			//// Set the uniform for the projection matrix
			//cam.Matrix(shader, "camMatrix");
			//shader.SetUniform1f("radius", maxRad);
			//shader.SetUniform1f("zoom", cam.zoomLevel);;

			//// Bind VAO
			//vao.Bind();


			//// Bind the appropriate VBO
			//glBindBuffer(GL_ARRAY_BUFFER, vbos[currentBuffer].ID);

			//// Draw points
			//glDrawArrays(GL_POINTS, 0, data.size() / attribCount); // positions.size() / 2 for 2D

			//// Unbind VAO
			//vao.Unbind();
		// Update the vertex buffer
        // Update the vertex buffer
    vbos[currentBuffer].Bind();
    glBufferSubData(GL_ARRAY_BUFFER, 0, data.size() * sizeof(float), data.data());
    vbos[currentBuffer].Unbind();

    // If indices are used, update the element buffer as well
    if (!indices.empty())
    {
        ebos[currentBuffer].Bind();
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, indices.size() * sizeof(GLuint), indices.data());
        ebos[currentBuffer].Unbind();
    }

    // Switch to the other buffer for the next frame
    currentBuffer = (currentBuffer + 1) % 2;

    // Use the shader program
    shader.Bind();

    // Set the uniform for the projection matrix
    cam.Matrix(shader, "camMatrix");
    shader.SetUniform1f("radius", maxRad);
    shader.SetUniform1f("zoom", cam.zoomLevel);

    // Bind VAO
    vao.Bind();

    // Bind the correct VBO for the current buffer
    vbos[currentBuffer].Bind();

    // Re-specify the vertex attributes if necessary (only if the VBO changes)
    // If your VBOs have the same layout and you set up the VAO to use the attribute pointers correctly, you may not need to re-specify them.
    // For safety, you might re-bind the vertex attribute pointers here.

    // Bind the correct EBO for the current buffer
    if (!indices.empty())
    {
        ebos[currentBuffer].Bind();
    }

    // Draw using either glDrawElements or glDrawArrays based on whether EBO exists
    if (!indices.empty())
    {
        // Draw elements with EBO (using indices)
        glDrawElements(GL_LINES, indices.size(), GL_UNSIGNED_INT, 0);
    }
    else
    {
        // Draw points without EBO (no indices)
        glDrawArrays(GL_POINTS, 0, data.size() / attribCount);
    }

    // Unbind EBO (optional)
    if (!indices.empty())
    {
        ebos[currentBuffer].Unbind();
    }

    // Unbind VBO (optional)
    vbos[currentBuffer].Unbind();

    // Unbind VAO
    vao.Unbind();
}
