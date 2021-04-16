#pragma once

#include "Object.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>

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
	glm::mat3 strain;
	glm::vec3 n1, n2, n3, n4;
	float volume;

};

class ElasticManager : public Object
{
private:
	GLuint VAO;
	GLuint VBO, EBO;

	//fps
	float timestep = 1.f / 18000.f;
	
	bool enableDamping = true;
	
	// parameters
	float density = 1000;
	float gravity = -9.8f;
	float youngs = 2500000;
	float poisson = 0.25;
	float groundPlane = -3.0f;


	std::vector<Particle> Particles;
	std::vector<Simplex_3> Elements;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> StartPositions;

	int drawmode = 1; // 0 - points, 1 - lines, 2 - quads

public:

	ElasticManager();
	~ElasticManager();

	void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
	void update();

	void update_buffer();

	void add_cube(glm::vec3 coords, float edge);

	void genTetra(int p1, int p2, int p3, int p4);

	int get_timestep() { return timestep; };

};

