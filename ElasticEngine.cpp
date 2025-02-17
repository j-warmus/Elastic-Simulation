#include "ElasticEngine.h"

void ElasticEngine::advancePhysicsSim(float timestep)
{
	for (auto& tetra : tetraVec) {
		int p1, p2, p3, p4;
		glm::vec3 r1, r2, r3, r4;
		glm::vec3 e1, e2, e3;

		p1 = tetra.particleIdx[0];
		p2 = tetra.particleIdx[1];
		p3 = tetra.particleIdx[2];
		p4 = tetra.particleIdx[3];

		r1 = particleVec[p1].position;
		r2 = particleVec[p2].position;
		r3 = particleVec[p3].position;
		r4 = particleVec[p4].position;

		e1 = r1 - r4;
		e2 = r2 - r4;
		e3 = r3 - r4;

		glm::mat3 T = glm::mat3(e1, e2, e3);
		glm::mat3 F = T * tetra.t0inv;

		glm::mat3 total_strain = .5f * (glm::transpose(F) * F - glm::mat3(1.f));
		glm::mat3 elastic_strain = total_strain - tetra.plasticStrain;
	
		if (parameters.plasticDeformation) {
			float e_trace = elastic_strain[0][0] + elastic_strain[1][1] + elastic_strain[2][2];
			glm::mat3 d_elastic_strain = elastic_strain - e_trace / 3.f * glm::mat3(1.f);

			float mag_de_strain = PhysicsUtil::calcFrobeniusNorm(d_elastic_strain);

			glm::mat3 d_plastic = glm::mat3(0.f);
			if (mag_de_strain > parameters.elastic_limit) {
				d_plastic = ((mag_de_strain - parameters.elastic_limit) / mag_de_strain) * d_elastic_strain;
				tetra.plasticStrain = (tetra.plasticStrain + d_plastic) * fminf(1.f, parameters.plastic_limit / PhysicsUtil::calcFrobeniusNorm(tetra.plasticStrain + d_plastic));

			}
		}
		glm::mat3 strain = elastic_strain;
		float lame1 = (parameters.youngs * parameters.poisson) / ((1.f + parameters.poisson) * (1.f - 2.f * parameters.poisson));
		float lame2 = parameters.youngs / (2.f * (1.f + parameters.poisson));
		float trace = strain[0][0] + strain[1][1] + strain[2][2];


		glm::mat3 cauchy;
		// damping
		if (parameters.enableDamping == true)
		{
			r1 = particleVec[p1].velocity;
			r2 = particleVec[p2].velocity;
			r3 = particleVec[p3].velocity;
			r4 = particleVec[p4].velocity;

			e1 = r1 - r4;
			e2 = r2 - r4;
			e3 = r3 - r4;
			glm::mat3 dF = glm::mat3(e1, e2, e3) * tetra.t0inv;
			glm::mat3 dFt = glm::transpose(glm::mat3(e1, e2, e3)) * tetra.t0inv;
			glm::mat3 v = .5f * (dFt * dF);
			cauchy = 2.f * lame2 * strain + lame1 * trace * glm::mat3(1.f) + parameters.dampingFactor * v;

		}
		else {
			cauchy = 2.f * lame2 * strain + lame1 * trace * glm::mat3(1.f);

		}

		particleVec[p1].force += F * cauchy * tetra.n1;
		particleVec[p2].force += F * cauchy * tetra.n2;
		particleVec[p3].force += F * cauchy * tetra.n3;
		particleVec[p4].force += F * cauchy * tetra.n4;

		//if (parameters.	enableCollision) {
		//	for (auto& other_e : Elements) {
		//		for (int i = 0; i < 4; ++i) {
		//			auto i_vtx = e.particleIdx[i];
		//			if (p_in_tetra(Particles[i_vtx], other_e)) {
		//				glm::vec3 force = calc_force(Particles[i_vtx].position, other_e);
		//				Particles[i_vtx].force += force;
		//			}
		//		}
		//	}
		//}

	}

	// update particles
	for (auto& p : particleVec) {
		p.velocity += (timestep / p.mass) * p.force;
		p.velocity.y += timestep * parameters.gravity;
		// TODO is this supposed to be before??
		p.position += timestep * p.velocity;
		p.force = glm::vec3(0.f, 0.f, 0.f);
		// p.velocity *= dampingFactor;

		if (p.position.y < parameters.groundPlane) {
			p.position.y = parameters.groundPlane;
			p.velocity = glm::vec3(0.f, 0.f, 0.f);
		}

	}

}

std::vector<glm::vec3> ElasticEngine::genVertices() const
{
	// This potentially gets called every frame. More robust solution would possibly have each 
	// particle index into a vertex vec with each of the positions and just return that as reference
	std::vector<glm::vec3> outVec;
	outVec.reserve(particleVec.size());
	for (auto& particle : particleVec) {
		outVec.push_back(particle.position);
	}

	return outVec;
}

std::vector<glm::ivec3> ElasticEngine::genIndices() const
{
	std::vector<glm::ivec3> outVec;
	outVec.reserve(tetraVec.size() * 4);

	// Note: this repeats faces.  Whole system is due for rewrite so will leave as is,
	// but has potential performance cost on more complex models
	for (auto& tetra : tetraVec) {
		outVec.push_back(glm::ivec3(tetra.particleIdx[0], tetra.particleIdx[1], tetra.particleIdx[2]));
		outVec.push_back(glm::ivec3(tetra.particleIdx[0], tetra.particleIdx[1], tetra.particleIdx[3]));
		outVec.push_back(glm::ivec3(tetra.particleIdx[0], tetra.particleIdx[2], tetra.particleIdx[3]));
		outVec.push_back(glm::ivec3(tetra.particleIdx[1], tetra.particleIdx[2], tetra.particleIdx[3]));
	}

	return outVec;
}

ElasticEngine::ElasticEngine(elasticParams params)
	: parameters(params)
{
}

ElasticEngine::~ElasticEngine()
{
}

void ElasticEngine::generateCubeGeometry(glm::vec3 startpos, unsigned int width, unsigned int height, unsigned int depth, float edgelength)
{
	// Generates particles in particleVec.  They are in a cube arrangement.
	// The location of each particle in particleVec is a flattened 3d representation, [x][y][z]

	particleVec.resize(width * height * depth);

	for (int z = 0; z < depth; z++)
	{
		for (int y = 0; y < height; y++)
		{
			for (int x = 0; x < width; x++)
			{
				// TODO can be done with emplace_back
				PhysicsUtil::Particle p;
				p.position = startpos + glm::vec3(x * edgelength, y * edgelength, z * edgelength);
				particleVec[x + y * width + z * width * depth] = p;
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
				cubeToTetras(glm::ivec3(i, j, k), width, height, depth);
			}
		}
	}
}

void ElasticEngine::cubeToTetras(glm::ivec3 startParticleIndex, unsigned int width, unsigned int height, unsigned int depth)
{
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

		auto& p1 = particleVec.at(tetra.particleIdx[0]);
		auto& p2 = particleVec.at(tetra.particleIdx[1]);
		auto& p3 = particleVec.at(tetra.particleIdx[2]);
		auto& p4 = particleVec.at(tetra.particleIdx[3]);

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

		tetra.t0inv = glm::inverse(glm::mat3(e1, e2, e3));

		tetra.n1 = .5f * glm::cross(r4 - r2, r3 - r2);
		tetra.n2 = .5f * glm::cross(r3 - r1, r4 - r1);
		tetra.n3 = .5f * glm::cross(r4 - r1, r2 - r1);
		tetra.n4 = .5f * glm::cross(r2 - r1, r3 - r1);

		// Initialize plastic strain
		tetra.plasticStrain = glm::mat3(0.f);

		// Initialize masses
		for (int i = 0; i < 4; i++) {
			particleVec[tetra.particleIdx[i]].mass += (parameters.density * tetra.volume) / 4.f;
		}

		tetraVec.push_back(tetra);
	}

}
