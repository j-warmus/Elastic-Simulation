#pragma once

#include "FParticle.h"
#include "SpatialTable.h"
#include "Object.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>
#include <random>



class FluidManager : public Object
{
private:
	GLuint VAO;
	GLuint VBO;

	glm::vec3 color;
	glm::vec3 origin;
	float pointSize = 3.f;
	// constants
	float timestep = 1.f / 36000.f;
	int spf = 300; //samples per frame

	float p0 = 1000.f;
	float k = 1e1;//stiffness, will probably need changing
	float v = 1.f; //viscosity, also needs change
	// NOTE this is in cm/s2, 
	float gravity = -980.f;

	// made in box, particles per side, so 10 = 1000 particles total
	int ppside = 5;
	// cms
	float fluid_width = 150;

	// this can go once it works
	bool debugflag = false;
	// cms
	float h = 2;
	//sus might need to change this
	float support = 2.f * h;
	// box settings
	glm::vec3 box_origin = glm::vec3(0, 0, 0);
	float box_width = 400;
	float box_height = 1000;
	float boundary_constant = 1000000000;

	// some precomputed coefs
	//float Wijcoef = (1.f / powf(h, 3.f)) * (3.f / (2.f * 3.14159f));
	//float dWijcoef = (1.f / powf(h, 4.f)) * (3.f / (2.f * 3.14159f));
	float Wijcoef = 15.f / 3.14159 / (pow(h, 6));
	float dWijcoef = -45.f/ 3.14159 / (pow(h, 6));
	std::vector<FParticle> Particles;

	std::vector<glm::vec3> vertices;

	//Hash table stuff
	SpatialTable* stable;
	int p1 = 73856093;
	int p2 = 19349663;
	int p3 = 83492791;
	float cell_spacing = support;
	int hash_map_size = 1e5;

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

