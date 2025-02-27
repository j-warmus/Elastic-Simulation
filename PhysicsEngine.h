#pragma once
#include <glm/glm.hpp>
#include <vector>

class IPhysicsEngine
{
public:
	virtual void advancePhysicsSim(const float timestep) = 0;
	virtual std::vector<glm::vec3> genVertices() const = 0;
	virtual std::vector<glm::ivec3> genIndices() const = 0;
	virtual ~IPhysicsEngine() {};
};

