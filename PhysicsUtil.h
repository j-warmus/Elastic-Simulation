#pragma once
#include "main.h"

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
		glm::mat3 t0inv; // TODO figure out what this actually does
		glm::mat3 strain, plasticStrain;
		glm::ivec4 particleIdx; // Vector of 4 indices into the Particle vector, the 4 componenent Particles
		glm::vec3 n1, n2, n3, n4;
		float volume;
	};

	// Epsilon to reduce self-intersection 
	constexpr float ERROR_EPSILON = 00000001.f;

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

	// TODO - names suck
	// Checks if point is on the same side of the plane defined by v1, v2, and v3 as v4
	inline bool isSameSide(const glm::vec3& point, 
		const glm::vec3& v1, const glm::vec3& v2, 
		const glm::vec3& v3, const glm::vec3& v4)
	{
		const glm::vec3 norm = glm::cross(v2 - v1, v3 - v1);
		const float t_dot = glm::dot(norm, v4 - v1);
		const float p_dot = glm::dot(norm, point - v1);
		
		return p_dot * t_dot > ERROR_EPSILON ? true : false;
	}
}