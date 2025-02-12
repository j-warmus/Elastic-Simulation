#pragma once
class RenderBackend
{
public:
	// Todo impplement this
	//virtual void renderFrame() = 0;
	virtual void draw(int elems_to_draw, const glm::mat4& view, const glm::mat4& projection, const glm::mat4& model, const glm::vec3& color) = 0;
	virtual void updateVertexBuffer(const std::vector<glm::vec3>& vertices) const = 0;
	// Todo this should probably be two functions
	virtual bool initializeShadersFromFile(const std::string& vertexShaderPath, const std::string& fragShaderPath) = 0;
	virtual ~RenderBackend() {};
};

