#pragma once

#include "Renderer.h"
#include "main.h"
#include "RenderBackend.h"
#include "OpenGlBackend.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>
// TODO remove this once its not needed for refactor
#include "PhysicsUtil.h"

struct Particle
{
	glm::vec3 position;
	glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 force = glm::vec3(0.f,0.f,0.f);
	float mass = 0;
};

struct Simplex_3
{
	int p_idx[4];
	glm::mat3 t0inv;
	glm::mat3 strain, plastic_strain;
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

	//fps
	// TODO: demagic this
	float timestep = 1.f / 18000.f;
	
	bool enableDamping = false;
	bool enableCollision = false;
	bool plasticDeformation = true;
	float dampingFactor = .99997f;
	
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
	std::vector<Simplex_3> Elements;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> indices;
	int drawmode = 2; // 0 - points, 1 - lines, 2 - quads

	bool p_in_tetra(Particle p, Simplex_3 t);

	glm::vec3 calc_force(glm::vec3 p, Simplex_3 t);


public:

	PhysicsRenderer();
	~PhysicsRenderer();

	// TODO: this HAS to be generic, for now use derived for testing
	

	void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) override;
	void update() override;
	void setViewDimensions(int width, int height);

	void update_buffer();

	void genMesh(glm::vec3 startpos, int w, int h, int d);

	void add_test_tetra(glm::vec3 startpos, float scale);

	void add_test_cube(glm::vec3 startpos, float scale);

	void add_cube(glm::vec3 topleft);

	void genTetra(int p1, int p2, int p3, int p4);

	int idx3d(glm::vec3 idx);

	int get_timestep() { return timestep; };

	
};


