#include "ElasticEngine.h"
#include "PhysicsEngine.h"
#include "PhysicsRenderer.h"
#include "RenderBackend.h"
#include <glm/glm.hpp>
#include <memory>

PhysicsRenderer::PhysicsRenderer(std::unique_ptr<IPhysicsEngine>&& physicsEngine, std::unique_ptr<IRenderBackend>&& renderBackend)
	: m_physicsEngine(std::move(physicsEngine)), m_renderBackend(std::move(renderBackend))
{
	m_vertices = m_physicsEngine->genVertices();
	m_indices = m_physicsEngine->genIndices();

	m_renderBackend->initBuffers(m_vertices, m_indices);
}

void PhysicsRenderer::draw(const glm::mat4& view, const glm::mat4& projection)
{
	// Update vertex buffer from physics backend
	updateBuffer();

	m_renderBackend->draw(m_indices.size(), view, projection, m_model, m_color);
}

void PhysicsRenderer::update(float timestep)
{
	m_physicsEngine->advancePhysicsSim(timestep);
}

void PhysicsRenderer::setViewDimensions(int width, int height)
{
	m_width = width;
	m_height = height;
	m_renderBackend->updateViewport(m_width, m_height);
}

void PhysicsRenderer::updateBuffer()
{
	m_vertices = m_physicsEngine->genVertices();
	m_renderBackend->updateVertexBuffer(m_vertices);
}