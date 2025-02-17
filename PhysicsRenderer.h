#pragma once

#include "Renderer.h"
#include "main.h"
#include "RenderBackend.h"
#include "OpenGlBackend.h"
#include <memory>
#include <vector>
// TODO remove this once its not needed for refactor
#include "PhysicsUtil.h"
#include <assert.h>

// Todo: It's possible to make the particles point directly at memory in the vertex buffer to skip updating it
struct Particle
{
	glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);;
	glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 force = glm::vec3(0.f,0.f,0.f);
	float mass = 0;
};

// Tetrahedron consisting of 4 particles. Particles are NOT exclusive to one Simplex
struct Tetra
{
	glm::mat3 t0inv; // TODO figure out what this actually does
	glm::mat3 strain, plasticStrain;
	glm::ivec4 particleIdx; // Vector of 4 indices into the Particle vector, the 4 componenent Particles
	glm::vec3 n1, n2, n3, n4;
	float volume;
};

class PhysicsRenderer : public Renderer
{
private:
	std::unique_ptr<RenderBackend> renderBackend;

	glm::mat4 m_model;
	glm::vec3 m_color;
	glm::mat4 m_view;
	glm::mat4 m_projection;

	int m_width;
	int m_height;
	
	bool enableDamping = true;
	bool enableCollision = false; // Runs glacially right now, needs to be disabled until better collision detection is implemented
	bool plasticDeformation = true;
	float dampingFactor = 1.f;
	
	// parameters
	float density = 1000;
	float gravity = -9.8f;
	float youngs = 2500000;
	float poisson = 0.25;
	float groundPlane = -3.0f;

	//plasticity
	float elastic_limit = 0.001f;
	float plastic_limit = 0.2f;
	// mesh (w x l x h in # of particles, not meters)
	glm::vec3 origin = glm::vec3(-3, 5, -8);
	int height = 5;
	int width = 5;
	int depth = 5;
	

	float edgelength = 1;

	std::vector<Particle> Particles;
	std::vector<Tetra> Elements;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> indices;

	bool p_in_tetra(const Particle& p, const Tetra& t);

	glm::vec3 calc_force(glm::vec3 p, const Tetra& t);


public:

	PhysicsRenderer();
	~PhysicsRenderer();

	void draw(const glm::mat4& view, const glm::mat4& projection) override;
	void update(float timestep) override;
	void setViewDimensions(int width, int height);

	void update_buffer();

	void genMesh(glm::vec3 startpos, int w, int h, int d);

	void add_cube(glm::vec3 topleft);

	Tetra genTetra(const glm::ivec4& indices);

	int idx3d(glm::vec3 idx);	
};


