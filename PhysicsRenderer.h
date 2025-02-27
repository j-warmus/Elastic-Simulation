#pragma once


#include "PhysicsEngine.h"
#include "RenderBackend.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>
#include <vector>

class PhysicsRenderer : public IRenderer
{
public:
	PhysicsRenderer(std::unique_ptr<IPhysicsEngine>&& physicsEngine, std::unique_ptr<IRenderBackend>&& renderBackend);
	~PhysicsRenderer() {};

	void draw(const glm::mat4& view, const glm::mat4& projection) override;
	void update(float timestep) override;
	void setViewDimensions(int width, int height);

	void updateBuffer();

private:
	std::unique_ptr<IPhysicsEngine> m_physicsEngine;
	std::unique_ptr<IRenderBackend> m_renderBackend;

	glm::mat4 m_model = glm::scale(glm::vec3(2.f));;
	glm::vec3 m_color = glm::vec3(1.0f, 1.0f, 1.0f);

	int m_width;
	int m_height;

	std::vector<glm::vec3> m_vertices;
	std::vector<glm::ivec3> m_indices;
};


