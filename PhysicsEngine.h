#pragma once
#include "main.h"
#include <vector>

class PhysicsEngine
{
public:
	virtual void advancePhysicsSim(float timestep) = 0;
	virtual std::vector<glm::vec3> genVertices() const = 0;
	virtual std::vector<glm::ivec3> genIndices() const = 0;
	virtual void generateCubeGeometry(glm::vec3 startpos,
		unsigned int width, unsigned int height, unsigned int depth, float edgelength) = 0;
	virtual ~PhysicsEngine() {};
};

