#pragma once
#include "PhysicsEngine.h"
#include "PhysicsUtil.h"
#include <glm/glm.hpp>
#include <vector>


struct elasticParams {
	float density = 1000;
	float gravity = -9.8f;
	float youngs = 2500000;
	float poisson = 0.25;

	float groundPlane = -3.0f;
	float dampingFactor = 1.f;

	float elastic_limit = 0.001f;
	float plastic_limit = 0.2f;

	bool enableDamping = true;
	bool enableCollision = false; // Runs glacially right now, needs to be disabled until better collision detection is implemented
	bool plasticDeformation = true;
};

class ElasticEngine : public IPhysicsEngine
{
public:
	ElasticEngine(elasticParams params);
	~ElasticEngine();

	void advancePhysicsSim(float timestep);

	// Generate vertex or index vector for usage by renderer
	std::vector<glm::vec3> genVertices() const;
	std::vector<glm::ivec3> genIndices() const;

	// Fills particle and tetra vecs, must be called before calling advancePhysicsSim
	// The whole sequence is kind of arcane but I want to implement a more general model loading solution so I'll leave as is
	// Generates a w x h x d cube centered on startpos. Each particle is edgelength apart.
	void generateCubeGeometry(glm::vec3 startpos,
		unsigned int width, unsigned int height, unsigned int depth, float edgelength);

private:
	std::vector<PhysicsUtil::Particle> particleVec;
	std::vector<PhysicsUtil::Tetra> tetraVec;
	elasticParams parameters;

	// helper function for generateCubeGeometry, generates 5 tetras from each 8-particle cube, starting with particle at given index
	void cubeToTetras(glm::ivec3 startParticleIndex, unsigned int width, unsigned int height, unsigned int depth);
};

