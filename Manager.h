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
	glm::vec3 velocity;
	glm::vec3 force;
	float mass;
};

struct Simplex_4
{
	uint32_t p_idx[4];
	glm::mat3 t0inv;
	glm::mat3 strain;
	glm::vec3 n0, n1, n2;
	float volume;

};

class ElasticManager : public Object
{
private:
	GLuint VAO;
	GLuint VBO, EBO;

	float timestep = 1.0f / 60.f;
	bool enableDamping = true;
	
	float groundPlane = -1.0f;
	std::vector<Particle> Particles;
	std::vector<Simplex_4> Elements;
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec3> StartPositions;

	int drawmode = 0; // 0 - points, 1 - lines, 2 - quads

public:

	ElasticManager();
	~ElasticManager();

	void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
	void update();

	void update_buffer();

};

