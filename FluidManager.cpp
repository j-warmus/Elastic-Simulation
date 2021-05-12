#include "FluidManager.h"

FluidManager::FluidManager(glm::vec3 position, glm::vec3 particle_color)
{
	model = glm::scale(glm::vec3(1.f));
	origin = position;
	color = particle_color;

	// Add elements to particle vector
	gen_fluid();

	// push particle positions onto vertices;
	for (auto& element : Particles) {
		vertices.push_back(element.position);
	}

	// Generate a Vertex Array (VAO) and Vertex Buffer Object (VBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	// Bind VAO
	glBindVertexArray(VAO);

	// Bind VBO to the bound VAO, and store the point data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
	// Enable Vertex Attribute 0 to pass point data through to the shader
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);

	// Unbind the VBO/VAO
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

FluidManager::~FluidManager()
{
	glDeleteBuffers(1, &VBO);
	glDeleteVertexArrays(1, &VAO);
}

void FluidManager::draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader)
{
	// Actiavte the shader program 
	glUseProgram(shader);

	// Get the shader variable locations and send the uniform data to the shader 
	glUniformMatrix4fv(glGetUniformLocation(shader, "view"), 1, false, glm::value_ptr(view));
	glUniformMatrix4fv(glGetUniformLocation(shader, "projection"), 1, false, glm::value_ptr(projection));
	glUniformMatrix4fv(glGetUniformLocation(shader, "model"), 1, GL_FALSE, glm::value_ptr(model));
	glUniform3fv(glGetUniformLocation(shader, "color"), 1, glm::value_ptr(color));

	update_buffer();

	// Bind the VAO
	glBindVertexArray(VAO);
	float pointSize = 1.0f;
	// Set point size
	glPointSize(pointSize);

	// Draw the points 
	glDrawArrays(GL_POINTS, 0, vertices.size());

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void FluidManager::update()
{
	for (auto& p : Particles) {
		p.position -= glm::vec3(0, 0.000001, 0);
	}

}

void FluidManager::update_buffer()
{
	for (unsigned i = 0; i < Particles.size(); i++) {
		vertices[i] = Particles[i].position;
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void FluidManager::gen_fluid()
{
	// TODO make this good lol
	FParticle* p;

	p = new FParticle;
	p->position = origin;
	Particles.push_back(*p);

}