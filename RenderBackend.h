#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
class IRenderBackend
{
public:
	virtual void draw(const int elems_to_draw, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec3& color) const = 0;
	virtual void updateVertexBuffer(const std::vector<glm::vec3>& vertices) const = 0;
	virtual void updateViewport(const int width, const int height) = 0;
	
	virtual void initBuffers(const std::vector<glm::vec3>& vertices, const std::vector<glm::ivec3>& indices) = 0;
	virtual ~IRenderBackend() {};
};

