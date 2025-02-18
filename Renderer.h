#pragma once
#include <glm/glm.hpp>

class IRenderer
{
public:
	virtual void draw(const glm::mat4& view, const glm::mat4& projection) = 0;
	virtual void update(float timestep) = 0;
	virtual void setViewDimensions(int width, int height) = 0;
	virtual ~IRenderer() {};
};

