#include "ElasticEngine.h"
#include "PhysicsUtil.h"
#include <assert.h>
#include <glm/glm.hpp>
#include <vector>

void ElasticEngine::advancePhysicsSim(const float timestep)
{
	for (auto& tetra : m_tetraVec) {

		PhysicsUtil::Particle& p1 = m_particleVec[tetra.particleIdx[0]];
		PhysicsUtil::Particle& p2 = m_particleVec[tetra.particleIdx[1]];
		PhysicsUtil::Particle& p3 = m_particleVec[tetra.particleIdx[2]];
		PhysicsUtil::Particle& p4 = m_particleVec[tetra.particleIdx[3]];

		const glm::vec3 r1 = p1.position;
		const glm::vec3 r2 = p2.position;
		const glm::vec3 r3 = p3.position;
		const glm::vec3 r4 = p4.position;

		const glm::vec3 e1 = r1 - r4;
		const glm::vec3 e2 = r2 - r4;
		const glm::vec3 e3 = r3 - r4;

		const glm::mat3 T = glm::mat3(e1, e2, e3);
		const glm::mat3 deformationGradient = T * tetra.inverseInitialT;

		// Right Cauchy-Green Tensor is Ft * F, totalStrain is Green-Lagrangian, note glm::mat3(1.f) is identity matrix
		const glm::mat3 totalStrain = .5f * (glm::transpose(deformationGradient) * deformationGradient - glm::mat3(1.f));
		const glm::mat3 elasticStrain = totalStrain - tetra.plasticStrain;
	
		const float elasticTrace = elasticStrain[0][0] + elasticStrain[1][1] + elasticStrain[2][2];

		// The exact origin of these equations I can no longer find
		// Essential idea is when stretched past a limit, the material will permanently deform
		if (m_elasticParams.plasticDeformation) {
			glm::mat3 dElasticStrain = elasticStrain - elasticTrace / 3.f * glm::mat3(1.f);

			const float magnitude_dElasticStrain = PhysicsUtil::calcFrobeniusNorm(dElasticStrain);

			glm::mat3 dPlasticStrain = glm::mat3(0.f);
			if (magnitude_dElasticStrain > m_elasticParams.elasticLimit) {
				dPlasticStrain = ((magnitude_dElasticStrain - m_elasticParams.elasticLimit) / magnitude_dElasticStrain) * dElasticStrain;
				tetra.plasticStrain = (tetra.plasticStrain + dPlasticStrain) * fminf(1.f, m_elasticParams.plasticLimit / PhysicsUtil::calcFrobeniusNorm(tetra.plasticStrain + dPlasticStrain));
			}
		}

		// Lame constants for Saint Venant-Kirchoff Model
		const float lame1 = (m_elasticParams.youngs * m_elasticParams.poisson) / 
			((1.f + m_elasticParams.poisson) * (1.f - 2.f * m_elasticParams.poisson));
		const float lame2 = m_elasticParams.youngs / (2.f * (1.f + m_elasticParams.poisson));

		// 2nd Piola-Kirchoff tensor from Saint Venant-Kirchoff Model
		const glm::mat3 cauchy = 2.f * lame2 * elasticStrain + lame1 * elasticTrace * glm::mat3(1.f);

		// Elastic force = deformation gradient * potential energy (1st Piola-Kirchoff tensor)
		p1.force += deformationGradient * cauchy * tetra.n1;
		p2.force += deformationGradient * cauchy * tetra.n2;
		p3.force += deformationGradient * cauchy * tetra.n3;
		p4.force += deformationGradient * cauchy * tetra.n4;
	}

	// Time integrate particles
	for (auto& p : m_particleVec) {
		p.velocity += (timestep / p.mass) * p.force;
		p.velocity.y += timestep * m_elasticParams.gravity;
		p.position += timestep * p.velocity;
		p.force = glm::vec3(0.f, 0.f, 0.f);

		if (p.position.y < m_elasticParams.groundPlane) {
			p.position.y = m_elasticParams.groundPlane;
			p.velocity = glm::vec3(0.f, 0.f, 0.f);
		}

	}

}

std::vector<glm::vec3> ElasticEngine::genVertices() const
{
	// This potentially gets called every frame. More robust solution would possibly have each 
	// particle index into a vertex vec with each of the positions and just return that as reference
	std::vector<glm::vec3> outVec;
	outVec.reserve(m_particleVec.size());
	for (auto& particle : m_particleVec) {
		outVec.push_back(particle.position);
	}

	return outVec;
}

std::vector<glm::ivec3> ElasticEngine::genIndices() const
{
	std::vector<glm::ivec3> outVec;
	outVec.reserve(m_tetraVec.size() * 4);

	// Note: this repeats faces.  Whole system is due for rewrite so will leave as is,
	// but has potential performance cost on more complex models
	for (auto& tetra : m_tetraVec) {
		outVec.push_back(glm::ivec3(tetra.particleIdx[0], tetra.particleIdx[1], tetra.particleIdx[2]));
		outVec.push_back(glm::ivec3(tetra.particleIdx[0], tetra.particleIdx[1], tetra.particleIdx[3]));
		outVec.push_back(glm::ivec3(tetra.particleIdx[0], tetra.particleIdx[2], tetra.particleIdx[3]));
		outVec.push_back(glm::ivec3(tetra.particleIdx[1], tetra.particleIdx[2], tetra.particleIdx[3]));
	}

	return outVec;
}

ElasticEngine::ElasticEngine(elasticParams elasticParameters, cubeParams cubeParameters)
	: m_elasticParams(elasticParameters), m_cubeParams(cubeParameters) {}

void ElasticEngine::generateCubeGeometry()
{
	// Generates particles in particleVec.  They are in a cube arrangement.
	// The location of each particle in particleVec is a flattened 3d representation, [x][y][z]
	auto& [origin, height, width, depth, edgelength] = m_cubeParams;

	m_particleVec.resize(width * height * depth);

	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				// TODO can be done with emplace_back
				PhysicsUtil::Particle p;
				p.position = origin + glm::vec3(x * edgelength, y * edgelength, z * edgelength);
				m_particleVec[x + y * width + z * width * depth] = p;
			}
		}
	}

	// cubeToTetras goes in the negative y and z direction, hence the strange ordering
	for (int i = 0; i < width - 1; ++i)
	{
		for (int j = 1; j < height; ++j)
		{
			for (int k = 1; k < depth; ++k)
			{
				cubeToTetras(glm::ivec3(i, j, k));
			}
		}
	}
}

void ElasticEngine::cubeToTetras(const glm::ivec3 startParticleIndex)
{
	auto& [_, height, width, depth, __] = m_cubeParams;
/*
 * Cube indices used below, 0 being the location of startParticleIndex
 *    4----7
 *   /|   /|
 *  0-+--3 |
 *  | 5--+-6
 *  |/   |/
 *  1----2
 *
 */
	
	// Lambda to flatten and add offset
	auto idx3d = [&, startParticleIndex, width, height, depth](glm::ivec3 offset) {
		glm::ivec3 idx = startParticleIndex + offset;
		return idx[0] + idx[1] * width + idx[2] * width * depth;
		};

	// Gets all 8 particles indices of a cube
	int p0 = idx3d(glm::ivec3(0,0,0));
	int p1 = idx3d(glm::ivec3(0, -1, 0));
	int p2 = idx3d(glm::ivec3(1, -1, 0));
	int p3 = idx3d(glm::ivec3(1, 0, 0));
	int p4 = idx3d(glm::ivec3(0, 0, -1));
	int p5 = idx3d(glm::ivec3(0, -1, -1));
	int p6 = idx3d(glm::ivec3(1, -1, -1));
	int p7 = idx3d(glm::ivec3(1, 0, -1));

	// generate all 5 tetras formed by a cube. . .
	// is there a way to automate this?
	std::vector<glm::ivec4> tetrasToConstruct{
		glm::ivec4(p0,p1,p2,p5),
		glm::ivec4(p2,p5,p6,p7),
		glm::ivec4(p0,p2,p3,p7),
		glm::ivec4(p0,p4,p5,p7),
		glm::ivec4(p2,p0,p5,p7),
	};

	// take the particles that are part of each tetra and construct the tetra, push back to tetraVec
	for (int i = 0; i < tetrasToConstruct.size(); i++)
	{
		PhysicsUtil::Tetra tetra{};
		tetra.particleIdx = tetrasToConstruct[i];

		auto& p1 = m_particleVec.at(tetra.particleIdx[0]);
		auto& p2 = m_particleVec.at(tetra.particleIdx[1]);
		auto& p3 = m_particleVec.at(tetra.particleIdx[2]);
		auto& p4 = m_particleVec.at(tetra.particleIdx[3]);

		// calculate initial values
		glm::vec3 r1 = p1.position;
		glm::vec3 r2 = p2.position;
		glm::vec3 r3 = p3.position;
		glm::vec3 r4 = p4.position;

		glm::vec3 e1 = r1 - r4;
		glm::vec3 e2 = r2 - r4;
		glm::vec3 e3 = r3 - r4;

		tetra.volume = (1.f / 6.f) * glm::dot(glm::cross(e1, e2), e3);
		assert(tetra.volume > 0.f && "Volume less than 0 for a PhysicsUtil::Tetra");

		tetra.inverseInitialT = glm::inverse(glm::mat3(e1, e2, e3));

		tetra.n1 = .5f * glm::cross(r4 - r2, r3 - r2);
		tetra.n2 = .5f * glm::cross(r3 - r1, r4 - r1);
		tetra.n3 = .5f * glm::cross(r4 - r1, r2 - r1);
		tetra.n4 = .5f * glm::cross(r2 - r1, r3 - r1);

		tetra.plasticStrain = glm::mat3(0.f);

		// Initialize particle masses based on density and volume
		for (int i = 0; i < 4; i++) {
			m_particleVec[tetra.particleIdx[i]].mass += (m_elasticParams.density * tetra.volume) / 4.f;
		}

		m_tetraVec.push_back(tetra);
	}

}
