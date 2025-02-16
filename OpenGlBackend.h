#pragma once

#include "main.h"
#include "RenderBackend.h"
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

class OpenGlBackend : public RenderBackend
{
public:
	// TODO: probably should maintain it's own vertices and indices
	OpenGlBackend(const std::vector<glm::vec3>& vertices, const std::vector<glm::ivec3>& indices);
	//OpenGlBackend();
	~OpenGlBackend();
	//// TODO try catch here
	void draw(int elems_to_draw, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec3& color) override;
	bool initializeShadersFromFile(const std::string& vertexShaderPath, const std::string& fragShaderPath) override;
	void updateVertexBuffer(const std::vector<glm::vec3>& vertices) const override;
	void updateViewport(int width, int height);

private:
	void setOpenGlSettings() const;
	void printVersionInfo() const;

	GLuint shaderProgram = 0;
	GLuint VAO, VBO, EBO;
	// TOdo unhardcode
	int m_drawmode = 1; // 0 - points, 1 - lines, 2 - quads
};