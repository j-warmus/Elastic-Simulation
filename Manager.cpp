#include "Manager.h"



ElasticManager::ElasticManager()
{
	model = glm::scale(glm::vec3(2.f));

	color = glm::vec3(1.0f, 1.0f, 1.0f);

	//test cube code
	add_cube(glm::vec3(-1, 1, -1), 2.f);


	
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

	// Bind the VAO
	glBindVertexArray(VAO);

	// set drawing mode
	if (drawmode == 0) { glPolygonMode(GL_FRONT_AND_BACK, GL_POINT); }
	else if (drawmode == 1){ glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); }
	else { glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); }

	glDrawElements(GL_TRIANGLES, 3 * 4 * Elements.size(), GL_UNSIGNED_INT, 0);
	//glDrawArrays(GL_POINTS, 0, vertices.size());

	// Unbind the VAO and shader program
	glBindVertexArray(0);
	glUseProgram(0);
}

void ElasticManager::update()
{
	update_buffer();
	model = model * glm::rotate(glm::radians(0.1f), glm::vec3(0.0f, 1.0f, 0.0f));
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
	

	Simplex_4* s;

	std::vector<glm::ivec4> tetras{
		glm::ivec4(0,1,2,5),
		glm::ivec4(2,5,6,7),
		glm::ivec4(0,2,3,7),
		glm::ivec4(0,4,5,7),
		glm::ivec4(0,2,5,7),
	};
	
	for (int i = 0; i < tetras.size(); i++)
	{
		s = new Simplex_4;
		
		s->p_idx[0] = tetras[i][0];
		s->p_idx[1] = tetras[i][1];
		s->p_idx[2] = tetras[i][2];
		s->p_idx[3] = tetras[i][3];
		// TODO other simplex initialization
		Elements.push_back(*s);
	}


	
}