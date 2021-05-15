#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

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


