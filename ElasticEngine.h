#pragma once
#include "PhysicsEngine.h"
#include "PhysicsUtil.h"
#include <glm/glm.hpp>
#include <vector>

// Both these structs have default values that produce a good result to start
struct elasticParams {
	float density = 1000;
	float gravity = -9.8f;
	float youngs = 2500000;
	float poisson = 0.25;

	float groundPlane = -3.0f;

	float elasticLimit = 0.001f;

	float plasticLimit = 0.2f;
	bool plasticDeformation = true;
};

struct cubeParams {
	glm::vec3 origin = glm::vec3(-3, 5, -8);

	unsigned int height = 5;
	unsigned int width = 5;
	unsigned int depth = 5;
	float edgelength = 1;
};

class ElasticEngine : public IPhysicsEngine
{
public:
	ElasticEngine(elasticParams params = {}, cubeParams cubeParameters = {});
	~ElasticEngine() {};

	void advancePhysicsSim(const float timestep) override;

	// Generate vertex or index vector for usage by renderer
	std::vector<glm::vec3> genVertices() const override;
	std::vector<glm::ivec3> genIndices() const override;

	// Fills particle and tetra vecs, must be called before calling advancePhysicsSim
	// The whole sequence is kind of arcane but I want to implement a more general model loading solution so I'll leave as is
	// Generates a w x h x d cube centered on startpos. Each particle is edgelength apart.
	void generateCubeGeometry();

private:
	std::vector<PhysicsUtil::Particle> m_particleVec;
	std::vector<PhysicsUtil::Tetra> m_tetraVec;
	elasticParams m_elasticParams;
	cubeParams m_cubeParams;

	// helper function for generateCubeGeometry, generates 5 tetras from each 8-particle cube, starting with particle at given index
	void cubeToTetras(const glm::ivec3 startParticleIndex);
};

