#pragma once

#include <glm/glm.hpp>
#include "RenderBackend.h"
#include <GL/glew.h>
#include <string>
#include <vector>

// Must be initialized after Window object due to unfortunate GLFW and GLEW state dependency
class OpenGlBackend : public IRenderBackend
{
public:
	OpenGlBackend();
	~OpenGlBackend();

	void initBuffers(const std::vector<glm::vec3>& vertices, const std::vector<glm::ivec3>& indices);
	void draw(int elems_to_draw, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec3& color) override;
	bool initializeShadersFromFile(const std::string& vertexShaderPath, const std::string& fragShaderPath);
	void updateVertexBuffer(const std::vector<glm::vec3>& vertices) const override;
	void updateViewport(int width, int height);

private:
	void setOpenGlSettings() const;
	void printVersionInfo() const;

	GLuint shaderProgram = 0;
	GLuint VAO, VBO, EBO;

	int m_drawmode = 1; // 0 - points, 1 - lines, 2 - quads
};