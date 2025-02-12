#include "PhysicsRenderer.h"



PhysicsRenderer::PhysicsRenderer()
{
	m_model = glm::scale(glm::vec3(2.f));

	m_color = glm::vec3(1.0f, 1.0f, 1.0f);


	genMesh(origin + glm::vec3(0,0,0), width, height, depth);
	// x + y * w + z * w * d
	// this is like super messy because add_cube goes in the negative y and z direction, I'll fix this later

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
	//add_test_cube(origin, 2);
	//add_test_cube(origin + glm::vec3(1,10,0), 2);
	////tetras for collision testing
	//for (int i = 0; i < 10; i++) {
	//	add_test_tetra(origin + glm::vec3(0, 5 + 3*i, 1), 2.f);
	//	add_test_tetra(origin + glm::vec3(2, 3 + 3 * i, 2), 2.f);
	//	add_test_tetra(origin + glm::vec3(-2, 1 + 3 * i, 1), 2.f);

	//}

	//add_test_tetra(origin + glm::vec3(0,50,0), 2.f);



	for (auto& element : Particles) {
		vertices.push_back(element.position);
	}


	// create EBO from simplexes
	std::vector<glm::ivec3> indices{};

	for (auto& element : Elements) {
		indices.push_back(glm::ivec3(element.p_idx[0], element.p_idx[1], element.p_idx[2]));
		indices.push_back(glm::ivec3(element.p_idx[0], element.p_idx[1], element.p_idx[3]));
		indices.push_back(glm::ivec3(element.p_idx[0], element.p_idx[2], element.p_idx[3]));
		indices.push_back(glm::ivec3(element.p_idx[1], element.p_idx[2], element.p_idx[3]));
	}

	renderBackend = std::make_unique<OpenGlBackend>(vertices,indices);
	// TODO unhardcode this
	renderBackend->initializeShadersFromFile("shaders/shader.vert", "shaders/shader.frag");
	//// Generate a vertex array (VAO) and vertex buffer object (VBO).
	//glGenVertexArrays(1, &VAO);
	//glGenBuffers(1, &VBO);

	//// Bind to the VAO.
	//glBindVertexArray(VAO);

	//// Bind VBO to the bound VAO, and store the vertex data
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	//// Enable Vertex Attribute 0 to pass the vertex data through to the shader
	//glEnableVertexAttribArray(0);
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	////// Generate EBO, bind the EBO to the bound VAO, and send the index data
	//glGenBuffers(1, &EBO);
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

	//// Unbind the VBO/VAO
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}



PhysicsRenderer::~PhysicsRenderer()
{
	//glDeleteBuffers(1, &VBO);
	//glDeleteBuffers(1, &EBO);
	//glDeleteVertexArrays(1, &VAO);
}

void PhysicsRenderer::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader)
{
	//// Actiavte the shader program 
	//glUseProgram(shader);

	//// Get the shader variable locations and send the uniform data to the shader 
	//glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
	//glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
	//glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	//glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

	// update vertex positions
	update_buffer();

	renderBackend->draw(Elements.size(), view, projection, m_model, m_color);

	//// Bind the VAO
	//glBindVertexArray(VAO);

	//// set drawing mode
	//if (drawmode == 0) { glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); }
	//else if (drawmode == 1) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	//else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	//glDrawElements(GL_TRIANGLES, 3 * 4 * Elements.size(), GL_UNSIGNED_INT, 0);
	////glDrawArrays(GL_POINTS, 0, vertices.size());

	//// Unbind the VAO and shader program
	//glBindVertexArray(0);
	//glUseProgram(0);
}

void PhysicsRenderer::update()
{
	// update elements
	int p1, p2, p3, p4;
	glm::vec3 r1, r2, r3, r4;
	glm::vec3 e1, e2, e3;
	for (auto& e : Elements) {
		p1 = e.p_idx[0];
		p2 = e.p_idx[1];
		p3 = e.p_idx[2];
		p4 = e.p_idx[3];

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
		glm::mat3 elastic_strain = total_strain - e.plastic_strain;
		// plastic deformation occurs here
		if (plasticDeformation) {
			float e_trace = elastic_strain[0][0] + elastic_strain[1][1] + elastic_strain[2][2];
			glm::mat3 d_elastic_strain = elastic_strain - e_trace / 3.f * glm::mat3(1.f);

			float mag_de_strain = frob_norm(d_elastic_strain);

			glm::mat3 d_plastic = glm::mat3(0.f);
			if (mag_de_strain > elastic_limit) {
				d_plastic = ((mag_de_strain - elastic_limit) / mag_de_strain) * d_elastic_strain;
				e.plastic_strain = (e.plastic_strain + d_plastic) * fminf(1.f, plastic_limit / frob_norm(e.plastic_strain + d_plastic));

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

		//collision
		if (enableCollision){
			for (auto& other_e : Elements) {
				for (auto& i_vtx : e.p_idx) {
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

void PhysicsRenderer::update_buffer()
{
	// Todo ???
	vertices.clear();
	for (auto& element : Particles) {
		vertices.push_back(element.position);
	}

	renderBackend->updateVertexBuffer(vertices);

	//glBindVertexArray(VAO);
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	//glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), vertices.data());
	//glBindBuffer(GL_ARRAY_BUFFER, 0);
	//glBindVertexArray(0);
}

//Adds particles to the particle vector.  does it in x, y, z order.  Resizes to w * h *d

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

// Adds a single tetra. For collision testing.
void PhysicsRenderer::add_test_tetra(glm::vec3 startpos, float scale) {
	
	Particle* p;
	int idx = Particles.size();

	p = new Particle;
	p->position = startpos;
	Particles.push_back(*p);

	p = new Particle;
	p->position = startpos + glm::vec3(1, 0, 0) * scale;
	Particles.push_back(*p);

	p = new Particle;
	p->position = startpos + glm::vec3(.5, 1, 0) * scale;
	Particles.push_back(*p);

	p = new Particle;
	p->position = startpos + glm::vec3(.5, .5, -1) * scale;;
	Particles.push_back(*p);

	genTetra(idx, idx + 1, idx + 2, idx + 3);

}

// Adds a single cube with 4 tetras.
void PhysicsRenderer::add_test_cube(glm::vec3 startpos, float scale) {
	Particle* p;
	int idx = Particles.size();

	std::vector<glm::vec3> cubepts;

	cubepts.push_back(startpos);
	cubepts.push_back(startpos + scale * glm::vec3(0, -1, 0));
	cubepts.push_back(startpos + scale * glm::vec3(1, -1, 0));
	cubepts.push_back(startpos + scale * glm::vec3(1, 0, 0));
	cubepts.push_back(startpos + scale * glm::vec3(0, 0, -1));
	cubepts.push_back(startpos + scale * glm::vec3(0, -1, -1));
	cubepts.push_back(startpos + scale * glm::vec3(1, -1, -1));
	cubepts.push_back(startpos + scale * glm::vec3(1, 0, -1));

	for (auto& pos : cubepts) {
		p = new Particle;
		p->position = pos;
		Particles.push_back(*p);
	}

	std::vector<glm::ivec4> tetras{
	glm::ivec4(idx,idx + 1,idx + 2,idx + 5),
	glm::ivec4(idx + 2,idx + 5,idx + 6,idx + 7),
	glm::ivec4(idx + 0,idx + 2,idx + 3,idx + 7),
	glm::ivec4(idx + 0,idx + 4,idx + 5,idx + 7),
	glm::ivec4(idx + 2,idx + 0,idx + 5,idx + 7),
	};

	for (int i = 0; i < tetras.size(); i++)
	{
		genTetra(tetras[i][0], tetras[i][1], tetras[i][2], tetras[i][3]);
	}
}

// From the idx of the top left particle, makes tetras out of the below and in front.  
//TODO 
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
	
	int p0 = idx3d(topleft);
	int p1 = idx3d(topleft + glm::vec3(0,-1,0));
	int p2 = idx3d(topleft + glm::vec3(1, -1, 0));
	int p3 = idx3d(topleft + glm::vec3(1, 0, 0));
	int p4 = idx3d(topleft + glm::vec3(0, 0, -1));
	int p5 = idx3d(topleft + glm::vec3(0, -1, -1));
	int p6 = idx3d(topleft + glm::vec3(1, -1, -1));
	int p7 = idx3d(topleft + glm::vec3(1, 0, -1));


	std::vector<glm::ivec4> tetras{
		glm::ivec4(p0,p1,p2,p5),
		glm::ivec4(p2,p5,p6,p7),
		glm::ivec4(p0,p2,p3,p7),
		glm::ivec4(p0,p4,p5,p7),
		glm::ivec4(p2,p0,p5,p7),
	};

	for (int i = 0; i < tetras.size(); i++)
	{
		genTetra(tetras[i][0], tetras[i][1], tetras[i][2], tetras[i][3]);
	}


	
}

// Takes 4 particle idx's and creates a tetra.  Pushes the tetra to Elements.
void PhysicsRenderer::genTetra(int p1, int p2, int p3, int p4) 
{
	Simplex_3* s = new Simplex_3;
	s->p_idx[0] = p1;
	s->p_idx[1] = p2;
	s->p_idx[2] = p3;
	s->p_idx[3] = p4;

	// calculate initial values
	glm::vec3 r1 = Particles[p1].position;
	glm::vec3 r2 = Particles[p2].position;
	glm::vec3 r3 = Particles[p3].position;
	glm::vec3 r4 = Particles[p4].position;

	glm::vec3 e1 = r1 - r4;
	glm::vec3 e2 = r2 - r4;
	glm::vec3 e3 = r3 - r4;
	
	s->volume = (1.f / 6.f) * glm::dot(glm::cross(e1, e2), e3);
	if (s->volume < 0) { printf("Volume below 0 error. \n"); }

	//TODO make sure this isn't incorrect
	s->t0inv = glm::inverse(glm::mat3(e1, e2, e3));

	s->n1 = .5f * glm::cross(r4 - r2, r3 - r2);
	s->n2 = .5f * glm::cross(r3 - r1, r4 - r1);
	s->n3 = .5f * glm::cross(r4 - r1, r2 - r1);
	s->n4 = .5f * glm::cross(r2 - r1, r3 - r1);
	//printf("%f %f %f\n", s->n1.x, s->n1.y, s->n1.z);
	//printf("%f %f %f\n", s->n2.x, s->n2.y, s->n2.z);
	//printf("%f %f %f\n", s->n3.x, s->n3.y, s->n3.z);
	//printf("%f %f %f\n", s->n4.x, s->n3.y, s->n3.z);
	//printf("%f\n", s->volume);

	// Initialize plastic strain
	s->plastic_strain = glm::mat3(0.f);

	// Initialize masses
	for (int i = 0; i < 4; i++) {
		Particles[s->p_idx[i]].mass += (density * s->volume) / 4.f;
	}

	Elements.push_back(*s);

}	

int PhysicsRenderer::idx3d(glm::vec3 idx) {
	return idx.x + idx.y * width + idx.z * width * depth;
}

//check if point is on same side as remaining vertex (v4 is same side as p relative to 
bool PhysicsRenderer::same_side(glm::vec3 pos, glm::vec3 v1, glm::vec3 v2, glm::vec3 v3, glm::vec3 v4) {
	glm::vec3 norm = glm::cross(v2 - v1, v3 - v1);
	float t_dot = glm::dot(norm, v4 - v1);
	float p_dot = glm::dot(norm, pos - v1);
	// Epsilon just to reduce likelyhood of self-intersection or other funky business
	if (p_dot * t_dot > 00000001.f) { return true; }
	else { return false; }
}

//check if point is in tetra
bool PhysicsRenderer::p_in_tetra(Particle p, Simplex_3 t) {
	glm::vec3 pos = p.position;
	glm::vec3 v1 = Particles[t.p_idx[0]].position;
	glm::vec3 v2 = Particles[t.p_idx[1]].position;
	glm::vec3 v3 = Particles[t.p_idx[2]].position;
	glm::vec3 v4 = Particles[t.p_idx[3]].position;

	return same_side(v1, v2, v3, v4, pos) && same_side(v2, v3, v4, v1, pos) && same_side(v3, v4, v1, v2, pos) && same_side(v4, v1, v2, v3, pos);
}

// calculate centroid of simplex, repusle point away.  Coulombs law based.  Should only be used if intersection exists
glm::vec3 PhysicsRenderer::calc_force(glm::vec3 p, Simplex_3 t) 
{
	// this will need to be adjusted
	float force = 100000.f;

	int p1 = t.p_idx[0];
	int p2 = t.p_idx[1];
	int p3 = t.p_idx[2];
	int p4 = t.p_idx[3];

	glm::vec3 centroid = (Particles[p1].position + Particles[p3].position + Particles[p2].position + Particles[p4].position) / 4.f;
	float dist = glm::length(centroid - p);

	return glm::normalize(p - centroid) * force/dist;

	//TODO: maybe instead make this toward nearest face


}

float PhysicsRenderer::frob_norm(glm::mat3 mat) {
	float sum = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			sum += mat[i][j] * mat[i][j];
		}
	}
	return sqrt(sum);
}