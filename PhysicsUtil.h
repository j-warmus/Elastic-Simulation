#pragma once
#include <glm/glm.hpp>
#include <cmath>

namespace PhysicsUtil {
	struct Particle
	{
		glm::vec3 position = glm::vec3(0.f, 0.f, 0.f);;
		glm::vec3 velocity = glm::vec3(0.f, 0.f, 0.f);
		glm::vec3 force = glm::vec3(0.f, 0.f, 0.f);
		float mass = 0;
	};
	// Tetrahedron consisting of 4 particles. Particles are NOT exclusive to one Simplex
	struct Tetra
	{
		glm::mat3 inverseInitialT;	// Stored inverse initial basis vectors, used for deformation calculation
		glm::mat3 strain, plasticStrain;
		glm::ivec4 particleIdx;		// Vector of 4 indices into the Particle vector, the 4 componenent Particles
		glm::vec3 n1, n2, n3, n4;	// Normal vectors
		float volume;
	};

	// Square root of the sum of the absolute squares of matrix elements
	inline float calcFrobeniusNorm(const glm::mat3& mat){
		float sum = 0;
		for (int i = 0; i < 3; ++i) {
			for (int j = 0; j < 3; ++j) {
				sum += mat[i][j] * mat[i][j];
			}
		}
		return sqrt(sum);
	}
}