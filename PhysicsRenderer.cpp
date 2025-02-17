#include "PhysicsRenderer.h"

PhysicsRenderer::PhysicsRenderer()
{
	m_model = glm::scale(glm::vec3(2.f));

	m_color = glm::vec3(1.0f, 1.0f, 1.0f);


	// PROBLEM: physics renderer class relies on vertices and indices.  
	// Shouldn't actually care about model, the elastics renderer can handle model
	// Model loading has two parts: generating the particles, and generating the tetras.  Both are specific to the handler
	// Right now, doesn't have to be abstract.  Later, could be.
	// Ideally, something like loadmodel and gentetrasfrom model generically
	// 
	// Todo This should probably use a model loader interface.
	genMesh(origin + glm::vec3(0,0,0), width, height, depth);
	// x + y * w + z * w * d
	// this is super messy because add_cube goes in the negative y and z direction, I'll fix this later

	for (int i = 0; i < width - 1; i++)
	{
		for (int j = 1; j < height; j++)
		{
			for (int k = 1; k < depth; k++)
			{
				add_cube(glm::vec3(i, j, k));
			}
		}
	}

	for (auto& element : Particles) {
		vertices.push_back(element.position);
	}


	// create EBO from simplexes
	std::vector<glm::ivec3> indices{};

	// NOTE: this has repeats
	for (auto& element : Elements) {
		indices.push_back(glm::ivec3(element.particleIdx[0], element.particleIdx[1], element.particleIdx[2]));
		indices.push_back(glm::ivec3(element.particleIdx[0], element.particleIdx[1], element.particleIdx[3]));
		indices.push_back(glm::ivec3(element.particleIdx[0], element.particleIdx[2], element.particleIdx[3]));
		indices.push_back(glm::ivec3(element.particleIdx[1], element.particleIdx[2], element.particleIdx[3]));
	}

	renderBackend = std::make_unique<OpenGlBackend>(vertices,indices);
	// TODO unhardcode this
	renderBackend->initializeShadersFromFile("shaders/shader.vert", "shaders/shader.frag");
}



PhysicsRenderer::~PhysicsRenderer()
{
}

void PhysicsRenderer::draw(const glm::mat4& view, const glm::mat4& projection)
{
	// update vertex positions
	update_buffer();

	renderBackend->draw(Elements.size(), view, projection, m_model, m_color);
}

// Todo: this is parallelizable but nontrivially.  Need to ensure particle force update is atomic
void PhysicsRenderer::update(float timestep)
{
	for (auto& e : Elements) {
		int p1, p2, p3, p4;
		glm::vec3 r1, r2, r3, r4;
		glm::vec3 e1, e2, e3;

		p1 = e.particleIdx[0];
		p2 = e.particleIdx[1];
		p3 = e.particleIdx[2];
		p4 = e.particleIdx[3];

		r1 = Particles[p1].position;
		r2 = Particles[p2].position;
		r3 = Particles[p3].position;
		r4 = Particles[p4].position;

		e1 = r1 - r4;
		e2 = r2 - r4;
		e3 = r3 - r4;

		glm::mat3 T = glm::mat3(e1, e2, e3);
		glm::mat3 F = T * e.t0inv;

		glm::mat3 total_strain = .5f * (glm::transpose(F) * F - glm::mat3(1.f));
		glm::mat3 elastic_strain = total_strain - e.plasticStrain;
		// plastic deformation occurs here
		if (plasticDeformation) {
			float e_trace = elastic_strain[0][0] + elastic_strain[1][1] + elastic_strain[2][2];
			glm::mat3 d_elastic_strain = elastic_strain - e_trace / 3.f * glm::mat3(1.f);

			float mag_de_strain = PhysicsUtil::calcFrobeniusNorm(d_elastic_strain);

			glm::mat3 d_plastic = glm::mat3(0.f);
			if (mag_de_strain > elastic_limit) {
				d_plastic = ((mag_de_strain - elastic_limit) / mag_de_strain) * d_elastic_strain;
				e.plasticStrain = (e.plasticStrain + d_plastic) * fminf(1.f, plastic_limit / PhysicsUtil::calcFrobeniusNorm(e.plasticStrain + d_plastic));

			}
		}
		glm::mat3 strain = elastic_strain;
		float lame1 = (youngs * poisson) / ((1.f + poisson) * (1.f - 2.f * poisson));
		float lame2 = youngs / (2.f * (1.f + poisson));
		float trace = strain[0][0] + strain[1][1] + strain[2][2];


		glm::mat3 cauchy;
		// damping
		if (enableDamping == true)
		{
			r1 = Particles[p1].velocity;
			r2 = Particles[p2].velocity;
			r3 = Particles[p3].velocity;
			r4 = Particles[p4].velocity;

			e1 = r1 - r4;
			e2 = r2 - r4;
			e3 = r3 - r4;
			glm::mat3 dF = glm::mat3(e1, e2, e3) * e.t0inv;
			glm::mat3 dFt = glm::transpose(glm::mat3(e1, e2, e3)) * e.t0inv;
			glm::mat3 v = .5f * (dFt * dF);
			cauchy = 2.f * lame2 * strain + lame1 * trace * glm::mat3(1.f) + dampingFactor * v;

		}
		else {
			cauchy = 2.f * lame2 * strain + lame1 * trace * glm::mat3(1.f);

		}

		Particles[p1].force += F * cauchy * e.n1;
		Particles[p2].force += F * cauchy * e.n2;
		Particles[p3].force += F * cauchy * e.n3;
		Particles[p4].force += F * cauchy * e.n4;

		if (enableCollision){
			for (auto& other_e : Elements) {
				for (int i = 0; i < 4; ++i) {
					auto i_vtx = e.particleIdx[i];
					if (p_in_tetra(Particles[i_vtx], other_e)) {
						glm::vec3 force = calc_force(Particles[i_vtx].position, other_e);
						Particles[i_vtx] .force += force;
					}
				}
			}
		}

	}

	// update particles
	for (auto& p : Particles) {
		p.velocity += (timestep / p.mass) * p.force;
		p.velocity.y += timestep * gravity;
		p.position += timestep * p.velocity;
		p.force = glm::vec3(0.f,0.f,0.f);
		// p.velocity *= dampingFactor;

		if (p.position.y < groundPlane) {
			p.position.y = groundPlane;
			p.velocity = glm::vec3(0.f,0.f,0.f);
		}

	}

}

void PhysicsRenderer::setViewDimensions(int width, int height)
{
	m_width = width;
	m_height = height;
	renderBackend->updateViewport(m_width, m_height);
}

void PhysicsRenderer::update_buffer()
{
	// Todo ???
	vertices.clear();
	for (auto& element : Particles) {
		vertices.push_back(element.position);
	}

	renderBackend->updateVertexBuffer(vertices);
}

//Adds particles to the particle vector.  does it in x, y, z order.  Resizes to w * h *d

// Creates grid of particles. Should be called something like genParticles or genVertices?

// TODO: use an arbitrary particle vector.  Then we can fold into our particles struct after.
void PhysicsRenderer::genMesh(glm::vec3 startpos, int w, int h, int d) {

	Particle* p;
	Particles.resize(w * h * d);

	for (int x = 0; x < w; x++)
	{
		for (int y = 0; y < h; y++) 
		{
			for (int z = 0; z < d; z++) 
			{
				p = new Particle;
				p->position = startpos + glm::vec3(x * edgelength, y * edgelength, z * edgelength);
				Particles[x + y * w + z * w * d] = *p;
			}
		}
	}
}

// From the idx of the top left particle, makes tetras out of the below and in front.  
//TODO 

// This is insanely arcane. 
void PhysicsRenderer::add_cube(glm::vec3 topleft)
{
	/*
 * Cube indices used below.
 *    4----7
 *   /|   /|
 *  0-+--3 |
 *  | 5--+-6
 *  |/   |/
 *  1----2
 *
 */
	// Gets all 8 particles indices of a cube
	int p0 = idx3d(topleft);
	int p1 = idx3d(topleft + glm::vec3(0,-1,0));
	int p2 = idx3d(topleft + glm::vec3(1, -1, 0));
	int p3 = idx3d(topleft + glm::vec3(1, 0, 0));
	int p4 = idx3d(topleft + glm::vec3(0, 0, -1));
	int p5 = idx3d(topleft + glm::vec3(0, -1, -1));
	int p6 = idx3d(topleft + glm::vec3(1, -1, -1));
	int p7 = idx3d(topleft + glm::vec3(1, 0, -1));

	// generate all 5 tetras formed by a cube. . .
	// is there a way to automate this?
	std::vector<glm::ivec4> tetras{
		glm::ivec4(p0,p1,p2,p5),
		glm::ivec4(p2,p5,p6,p7),
		glm::ivec4(p0,p2,p3,p7),
		glm::ivec4(p0,p4,p5,p7),
		glm::ivec4(p2,p0,p5,p7),
	};

	for (int i = 0; i < tetras.size(); i++)
	{
		Elements.push_back(genTetra(tetras[i]));
	}


	
}

// Takes 4 particle idx's and creates a tetra.  Pushes the tetra to Elements.
Tetra PhysicsRenderer::genTetra(const glm::ivec4& indices) 
{
	Tetra s{};
	auto& p1 = Particles.at(indices[0]);
	auto& p2 = Particles.at(indices[1]);
	auto& p3 = Particles.at(indices[2]);
	auto& p4 = Particles.at(indices[3]);

	s.particleIdx = indices;

	// calculate initial values
	glm::vec3 r1 = p1.position;
	glm::vec3 r2 = p2.position;
	glm::vec3 r3 = p3.position;
	glm::vec3 r4 = p4.position;

	glm::vec3 e1 = r1 - r4;
	glm::vec3 e2 = r2 - r4;
	glm::vec3 e3 = r3 - r4;
	
	s.volume = (1.f / 6.f) * glm::dot(glm::cross(e1, e2), e3);
	assert(s.volume > 0.f && "Volume less than 0 for a Tetra");

	s.t0inv = glm::inverse(glm::mat3(e1, e2, e3));

	s.n1 = .5f * glm::cross(r4 - r2, r3 - r2);
	s.n2 = .5f * glm::cross(r3 - r1, r4 - r1);
	s.n3 = .5f * glm::cross(r4 - r1, r2 - r1);
	s.n4 = .5f * glm::cross(r2 - r1, r3 - r1);
	
	// Initialize plastic strain
	s.plasticStrain = glm::mat3(0.f);

	// Initialize masses
	for (int i = 0; i < 4; i++) {
		Particles[s.particleIdx[i]].mass += (density * s.volume) / 4.f;
	}

	return s;
}	

int PhysicsRenderer::idx3d(glm::vec3 idx) {
	return idx.x + idx.y * width + idx.z * width * depth;
}

//check if point is in tetra
// TODO: what is the particle reference here? needs to be stripped.  
bool PhysicsRenderer::p_in_tetra(const Particle& p, const Tetra& t) {
	glm::vec3 pos = p.position;
	glm::vec3 v1 = Particles[t.particleIdx[0]].position;
	glm::vec3 v2 = Particles[t.particleIdx[1]].position;
	glm::vec3 v3 = Particles[t.particleIdx[2]].position;
	glm::vec3 v4 = Particles[t.particleIdx[3]].position;

	return PhysicsUtil::isSameSide(v1, v2, v3, v4, pos) && PhysicsUtil::isSameSide(v2, v3, v4, v1, pos) && PhysicsUtil::isSameSide(v3, v4, v1, v2, pos) && PhysicsUtil::isSameSide(v4, v1, v2, v3, pos);
}

// calculate centroid of simplex, repusle point away.  Coulombs law based.  Should only be used if intersection exists
glm::vec3 PhysicsRenderer::calc_force(glm::vec3 p, const Tetra& t) 
{
	// this will need to be adjusted
	float force = 100000.f;

	int p1 = t.particleIdx[0];
	int p2 = t.particleIdx[1];
	int p3 = t.particleIdx[2];
	int p4 = t.particleIdx[3];

	glm::vec3 centroid = (Particles[p1].position + Particles[p3].position + Particles[p2].position + Particles[p4].position) / 4.f;
	float dist = glm::length(centroid - p);

	return glm::normalize(p - centroid) * force/dist;

	//TODO: maybe instead make this toward nearest face
}