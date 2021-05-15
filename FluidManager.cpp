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
	std::vector<int> j_idxs;
	float Wij;
	float pi;
	glm::vec3 pressure;
	glm::vec3 dWij;
	glm::vec3 d2v;
	float d_accum = 0.f;
	glm::vec3 dv;
	glm::vec3 xij;
	// First, find neighbors
	
	for (auto& p : Particles) {
		p.neighbors.clear();
		for (int i = 0; i < Particles.size(); i++) {
			if (&Particles[i] != &p) {
				if (glm::length(p.position - Particles[i].position) < support) {
					p.neighbors.push_back(i);
				}
			}
		}
		// Some skullduggery to make it faster, change this if neigbors arent correct
		//p.neighbors = j_idxs;
		//j_idxs.clear();
		//printf("%d\n", p.neighbors.size());
	}








	// precompute pi and Pi (pressure and density
	for (auto& p : Particles) {
		// density
		for (auto& j : p.neighbors) {
			Wij =  Wijcoef * cubicsplinekernel(glm::length(p.position - Particles[j].position) / h);
			
			d_accum += Wij * Particles[j].mass;
		}
		Wij = Wijcoef * cubicsplinekernel(glm::length(p.position - p.position) / h);
		p.density = d_accum + Wij * p.mass;
		d_accum = 0.f;
		p.pressure = k * (powf(p.density / p0, 7.f) - 1.f);
	}
	for (auto& p : Particles) {

		// pressure
		pressure = glm::vec3(0, 0, 0);
		d2v = glm::vec3(0, 0, 0);
		for (auto& j : p.neighbors) {
			dWij = dWijcoef * dcubicsplinekernel(glm::length(p.position - Particles[j].position) / h) * glm::normalize(p.position-Particles[j].position);
			pressure += Particles[j].mass * (p.pressure / (p.density * p.density) + Particles[j].pressure / (Particles[j].density * Particles[j].density)) * dWij;
			
			//viscosity precompute
			xij = p.position - Particles[j].position;
			d2v += (Particles[j].mass / Particles[j].density) * (p.velocity - Particles[j].velocity) * (glm::dot(xij, dWij) / (glm::dot(xij, xij) + 0.01f * (h * h)));
		}
		pressure *= p.density;
		
		p.force += -1.f * (p.mass) * pressure;

		//F viscosity here
		

		p.force += 2 * p.mass * v * d2v;

		// Gravity
		p.force.y += p.mass * gravity;







		//bounding code here
		if (p.position.y < box_origin.y - box_height / 2) {
			p.force.y += boundary_constant*(box_origin.y - box_height / 2 - p.position.y);
		}
		if (p.position.x < box_origin.x - box_width / 2) {
			p.force.x += boundary_constant * (box_origin.x - box_width / 2 - p.position.x);
		} else if (p.position.x > box_origin.x + box_width / 2) {
			p.force.x -= boundary_constant * (p.position.x - box_origin.x + box_width / 2);
		}

		if (p.position.z < box_origin.z - box_width / 2) {
			p.force.z += boundary_constant * (box_origin.z - box_width / 2 - p.position.z);
		}
		else if (p.position.z > box_origin.z + box_width / 2) {
			p.force.z -= boundary_constant * (p.position.z - box_origin.z + box_width / 2);
		}
		// Integrate
		p.velocity += (timestep / p.mass) * p.force;
		p.position += timestep * p.velocity;

		p.force = glm::vec3(0.f, 0.f, 0.f);
	}



}

void FluidManager::update_buffer()
{
	for (unsigned i = 0; i < Particles.size(); i++) {
		// put it back in meters or whatever
		vertices[i] = Particles[i].position/50.f;
	}

	glBindVertexArray(VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::vec3) * vertices.size(), vertices.data());
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void FluidManager::gen_fluid()
{
	
	FParticle* p;
	float d_mass = (4.f/3.f) * 3.14159 * p0 * h * h * h;
	std::random_device rd;  //Will be used to obtain a seed for the random number engine
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(-1.f, 1.f);
	for (int x = 0; x < ppside; x++) {
		for (int y = 0; y < ppside; y++) {
			for (int z = 0; z < ppside; z++) {
				p = new FParticle;
				p->position = origin + glm::vec3(-1 * fluid_width / 2 + fluid_width * x / ppside, -1 * fluid_width / 2 + fluid_width * y / ppside, -1 * fluid_width / 2 + fluid_width * z / ppside) + glm::vec3(dis(gen),dis(gen),dis(gen));
				p->mass = d_mass;
				Particles.push_back(*p);
			}
		}
	}
}

float FluidManager::cubicsplinekernel(float q) {
	if (q < 1) {
		return (2.f / 3.f) - q * q  + .5f * q * q * q;
	}
	else if (q < 2) {
		return (1.f / 6.f) * (2.f - q) * (2.f - q) * (2.f - q);
	}
	else {
		return 0.f;
	}
}

float FluidManager::dcubicsplinekernel(float q) {
	if (q < 1) {
		return q * (3.f*q - 4)/2.f;
	}
	else if (q < 2) {
		return -1.f * ((q - 2) * (q - 2) / 2.f);
	}
	else {
		return 0.f;
	}
}