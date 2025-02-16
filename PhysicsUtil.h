#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>

namespace PhysicsUtil {
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