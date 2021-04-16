#include "Manager.h"



ElasticManager::ElasticManager()
{
	model = glm::scale(glm::vec3(2.f));

	color = glm::vec3(1.0f, 1.0f, 1.0f);

	//test cube code
	add_cube(glm::vec3(-1, -1, -1), 2.f);


	for (auto& element : Particles) {
		vertices.push_back(element.position);
	}


	// create EBO from simplexes
	std::vector<glm::ivec3> indices;

	for (auto& element : Elements) {
		indices.push_back(glm::ivec3(element.p_idx[0], element.p_idx[1], element.p_idx[2]));
		indices.push_back(glm::ivec3(element.p_idx[0], element.p_idx[1], element.p_idx[3]));
		indices.push_back(glm::ivec3(element.p_idx[0], element.p_idx[2], element.p_idx[3]));
		indices.push_back(glm::ivec3(element.p_idx[1], element.p_idx[2], element.p_idx[3]));
	}

	// Generate a vertex array (VAO) and vertex buffer object (VBO).
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind to the VAO.
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the vertex data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass the vertex data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	//// Generate EBO, bind the EBO to the bound VAO, and send the index data
	glGenBuffers(1, &EBO);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(glm::ivec3) * indices.size(), indices.data(), GL_STATIC_DRAW);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}



ElasticManager::~ElasticManager()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);
	glDeleteVertexArrays(1, &VAO);
}

void ElasticManager::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader)
{
	// Actiavte the shader program 
	glUseProgram(shader);

	// Get the shader variable locations and send the uniform data to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

	// update vertex positions
	update_buffer();

	// Bind the VAO
	glBindVertexArray(VAO);

	// set drawing mode
	if (drawmode == 0) { glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); }
	else if (drawmode == 1) { glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	glDrawElements(GL_TRIANGLES, 3 * 4 * Elements.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, vertices.size());

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void ElasticManager::update()
{
	//model = model * glm::rotate(glm::radians(1.f), glm::vec3(0.0f, 1.0f, 0.0f));

	// update elements
	int p1, p2, p3, p4;
	glm::vec3 r1, r2, r3, r4;
	glm::vec3 e1, e2, e3;

	for (auto& e : Elements) {
		p1 = e.p_idx[0];
		p2 = e.p_idx[1];
		p3 = e.p_idx[2];
		p4 = e.p_idx[3];

		// calculate initial values
		r1 = Particles[p1].position;
		r2 = Particles[p2].position;
		r3 = Particles[p3].position;
		r4 = Particles[p4].position;

		e1 = r1 - r4;
		e2 = r2 - r4;
		e3 = r3 - r4;

		glm::mat3 T = glm::mat3(e1, e2, e3);
		glm::mat3 F = T * e.t0inv;

		glm::mat3 strain = .5f * (glm::transpose(F) * F - glm::mat3(1.f));

		float lame1 = (youngs * poisson) / ((1.f + poisson) * (1.f - 2.f * poisson));
		float lame2 = youngs / (2.f * (1.f + poisson));
		float trace = strain[0][0] + strain[1][1] + strain[2][2];

		glm::mat3 cauchy = 2.f * lame2 * strain + lame1 * trace * glm::mat3(1.f);

		Particles[p1].force += F * cauchy * e.n1;
		Particles[p2].force += F * cauchy * e.n2;
		Particles[p3].force += F * cauchy * e.n3;
		Particles[p4].force += F * cauchy * e.n4;

	}

	// update particles
	for (auto& p : Particles) {
		p.velocity += (timestep / p.mass) * p.force;
		p.velocity.y += timestep * gravity;
		p.position += timestep * p.velocity;
		p.force = glm::vec3(0.f,0.f,0.f);

		if (p.position.y < groundPlane) {
			p.position.y = groundPlane;
			p.velocity = glm::vec3(0.f,0.f,0.f);
		}

	}
}

void ElasticManager::update_buffer()
{
	vertices.clear();
	for (auto& element : Particles) {
		vertices.push_back(element.position);
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

// TODO: Split this into a cube gen and a func that works off of 8 part indices
void ElasticManager::add_cube(glm::vec3 coords, float edge)
{

	
	// adds a cube, coords is front left
	std::vector<glm::vec3> sverts{
		glm::vec3(coords.x, coords.y + edge, coords.z + edge),
		glm::vec3(coords.x, coords.y, coords.z + edge),
		glm::vec3(coords.x + edge, coords.y, coords.z + edge),
		glm::vec3(coords.x + edge, coords.y + edge, coords.z + edge),
		glm::vec3(coords.x, coords.y + edge, coords.z),
		glm::vec3(coords.x, coords.y, coords.z),
		glm::vec3(coords.x + edge, coords.y, coords.z),
		glm::vec3(coords.x + edge, coords.y + edge, coords.z)
	};

	Particle* p;

	for (auto& element : sverts) {
		p = new Particle;
		p->position = element;
		// TODO other particle initialization
		Particles.push_back(*p);
	}
	

	Simplex_3* s;

	size_t j = Particles.size() - 8;
	std::vector<glm::ivec4> tetras{
		glm::ivec4(j,j + 1,j + 2,j + 5),
		glm::ivec4(j + 2,j + 5,j + 6,j + 7),
		glm::ivec4(j,j + 2,j + 3,j + 7),
		glm::ivec4(j,j + 4,j + 5,j + 7),
		glm::ivec4(j+2,j,j + 5,j + 7),
	};

	for (int i = 0; i < tetras.size(); i++)
	{
		genTetra(tetras[i][0], tetras[i][1], tetras[i][2], tetras[i][3]);
	}


	
}

void ElasticManager::genTetra(int p1, int p2, int p3, int p4) 
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

	// Initialize masses
	for (int i = 0; i < 4; i++) {
		Particles[s->p_idx[i]].mass += (density * s->volume) / 4.f;
	}

	Elements.push_back(*s);

}	
