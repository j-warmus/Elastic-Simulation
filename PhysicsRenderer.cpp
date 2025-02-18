#include "ElasticEngine.h"
#include "OpenGlBackend.h"
#include "PhysicsRenderer.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <memory>

PhysicsRenderer::PhysicsRenderer()
{
	m_model = glm::scale(glm::vec3(2.f));

	m_color = glm::vec3(1.0f, 1.0f, 1.0f);

	elasticParams p{};
	physEngine = std::make_unique<ElasticEngine>(p);

	// TODO set these properly
	physEngine->generateCubeGeometry(origin, width, height, depth, edgelength);

	vertices = physEngine->genVertices();

	indices = physEngine->genIndices();

	// TODO this has to reflect the new values from the physics engine
	renderBackend = std::make_unique<OpenGlBackend>();
	renderBackend->initBuffers(vertices, indices);
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

	renderBackend->draw(indices.size(), view, projection, m_model, m_color);
}

void PhysicsRenderer::update(float timestep)
{
	physEngine->advancePhysicsSim(timestep);
}

void PhysicsRenderer::setViewDimensions(int width, int height)
{
	m_width = width;
	m_height = height;
	renderBackend->updateViewport(m_width, m_height);
}

void PhysicsRenderer::update_buffer()
{
	vertices = physEngine->genVertices();

	renderBackend->updateVertexBuffer(vertices);
}