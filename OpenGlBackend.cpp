#include "OpenGlBackend.h"
#include <glm/glm.hpp>
#include "GL/glew.h"
#include "shader.h"
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <glm/gtc/type_ptr.hpp>

OpenGlBackend::OpenGlBackend(const std::vector<glm::vec3>& vertices, const std::vector<glm::ivec3>& indices)
{
	setOpenGlSettings();
	printVersionInfo();

	// Generate a vertex array (VAO) and vertex buffer object (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass the vertex data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//// Generate EBO, bind the EBO to the bound VAO, and send the index data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

OpenGlBackend::~OpenGlBackend()
{
	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}



void OpenGlBackend::draw(int elems_to_draw, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec3& color)
{
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Actiavte the shader program 
	glUseProgram(shaderProgram);

	// Get the shader variable locations and send the uniform data to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, false, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(color));

	// Bind the VAO
	glBindVertexArray(VAO);

	// set drawing mode
	if (m_drawmode == 0) { glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); }
	else if (m_drawmode == 1) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	// TODO magic
	glDrawElements(GL_TRIANGLES, 3 * elems_to_draw, GL_UNSIGNED_INT, 0);

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

bool OpenGlBackend::initializeShadersFromFile(const std::string& vertexShaderPath, const std::string& fragShaderPath)
{
	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders(vertexShaderPath.c_str(), fragShaderPath.c_str());

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

// TODO this could be parsed out.
void OpenGlBackend::setOpenGlSettings() const
{
	// Enable depth buffering.
	glEnable(GL_DEPTH_TEST);

	// Related to shaders and z value comparisons for the depth buffer.
	glDepthFunc(GL_LEQUAL);

	// Set polygon drawing mode to fill front and back of each polygon.
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

	// Set clear color to black.
	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void OpenGlBackend::printVersionInfo() const
{
	// Get info of GPU and supported OpenGL version.
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "OpenGL version supported: " << glGetString(GL_VERSION)
		<< std::endl;

	//If the shading language symbol is defined.
#ifdef GL_SHADING_LANGUAGE_VERSION
	std::cout << "Supported GLSL version is: " <<
		glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
#endif
}

void OpenGlBackend::updateVertexBuffer(const std::vector<glm::vec3>& vertices) const
{
	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void OpenGlBackend::updateViewport(int width, int height)
{
	glViewport(0, 0, width, height);
}
