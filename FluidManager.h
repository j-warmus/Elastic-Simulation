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
	glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);;
	glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);
	glm::vec3 force = glm::vec3(0.f, 0.f, 0.f);
	float mass = 1.f;
	float density = 1.f;
	float pressure = 0.f;
	std::vector<int> neighbors;
};

class FluidManager : public Object
{
private:
	GLuint VAO;
	GLuint VBO;

	glm::vec3 color;
	glm::vec3 origin;
	float pointSize = 3.f;
	// constants
	float timestep = 1.f / 18000.f;
	int spf = 300; //samples per frame

	float p0 = 1.f;
	float k = 1.f;//stiffness, will probably need changing

	// NOTE this is in cm/s2, 
	float gravity = -980.f;

	// made in box, particles per side, so 10 = 1000 particles total
	int ppside = 5;
	// cms
	float fluid_width = 200;

	// this can go once it works
	bool debugflag = false;
	// cms
	float h = 1 * fluid_width / ppside;
	//sus might need to change this
	float support = 5.f * h;
	// box settings
	glm::vec3 box_origin = glm::vec3(0, 0, 0);
	float box_width = 400;
	float box_height = 1000;
	float boundary_constant = 1000000000;

	std::vector<FParticle> Particles;

	std::vector<glm::vec3> vertices;


public:



	FluidManager(glm::vec3 position, glm::vec3 particle_color);
	~FluidManager();

	void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader);
	void update();

	void update_buffer();
	void gen_fluid();

	float cubicsplinekernel(float q);

	float dcubicsplinekernel(float q);

	int get_spf(){ return spf; };
};

