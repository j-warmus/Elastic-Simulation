#pragma once
#include "Object.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>


struct FParticle
{
	glm::vec3 position;
	glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 force = glm::vec3(0.f, 0.f, 0.f);
	float mass = 0;
};

class FluidManager : public Object
{
private:
	GLuint VAO;
	GLuint VBO;

	glm::vec3 color;
	glm::vec3 origin;

	float timestep = 1.f / 18000.f;

	std::vector<FParticle> Particles;

	std::vector<glm::vec3> vertices;


public:
	
	FluidManager(glm::vec3 position, glm::vec3 particle_color);
	~FluidManager();

	void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
	void update();

	void update_buffer();
	void gen_fluid();
};

