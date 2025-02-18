#pragma once


#include "PhysicsEngine.h"
#include "RenderBackend.h"
#include "Renderer.h"
#include <glm/glm.hpp>
#include <memory>
#include <vector>

class PhysicsRenderer : public IRenderer
{
public:

	PhysicsRenderer();
	~PhysicsRenderer();

	void draw(const glm::mat4& view, const glm::mat4& projection) override;
	void update(float timestep) override;
	void setViewDimensions(int width, int height);

	void update_buffer();

private:
	std::unique_ptr<IPhysicsEngine> physEngine;
	std::unique_ptr<IRenderBackend> renderBackend;


	glm::mat4 m_model;
	glm::vec3 m_color;
	glm::mat4 m_view;
	glm::mat4 m_projection;

	int m_width;
	int m_height;

	// mesh (w x l x h in # of particles, not meters)
	glm::vec3 origin = glm::vec3(-3, 5, -8);
	int height = 5;
	int width = 5;
	int depth = 5;
	

	float edgelength = 1;

	std::vector<glm::vec3> vertices;
	std::vector<glm::ivec3> indices;

	//bool p_in_tetra(const Particle& p, const PhysicsUtil::PhysicsUtil::Tetra& t);

	//glm::vec3 calc_force(glm::vec3 p, const PhysicsUtil::PhysicsUtil::Tetra& t);
};


