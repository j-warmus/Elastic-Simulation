#include "PhysicsRenderer.h"

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
//
////check if point is in tetra
//// TODO: what is the particle reference here? needs to be stripped.  
//bool PhysicsRenderer::p_in_tetra(const Particle& p, const PhysicsUtil::Tetra& t) {
//	glm::vec3 pos = p.position;
//	glm::vec3 v1 = Particles[t.particleIdx[0]].position;
//	glm::vec3 v2 = Particles[t.particleIdx[1]].position;
//	glm::vec3 v3 = Particles[t.particleIdx[2]].position;
//	glm::vec3 v4 = Particles[t.particleIdx[3]].position;
//
//	return PhysicsUtil::isSameSide(v1, v2, v3, v4, pos) && PhysicsUtil::isSameSide(v2, v3, v4, v1, pos) && PhysicsUtil::isSameSide(v3, v4, v1, v2, pos) && PhysicsUtil::isSameSide(v4, v1, v2, v3, pos);
//}
//
//// calculate centroid of simplex, repusle point away.  Coulombs law based.  Should only be used if intersection exists
//glm::vec3 PhysicsRenderer::calc_force(glm::vec3 p, const PhysicsUtil::Tetra& t) 
//{
//	// this will need to be adjusted
//	float force = 100000.f;
//
//	int p1 = t.particleIdx[0];
//	int p2 = t.particleIdx[1];
//	int p3 = t.particleIdx[2];
//	int p4 = t.particleIdx[3];
//
//	glm::vec3 centroid = (Particles[p1].position + Particles[p3].position + Particles[p2].position + Particles[p4].position) / 4.f;
//	float dist = glm::length(centroid - p);
//
//	return glm::normalize(p - centroid) * force/dist;
//
//	//TODO: maybe instead make this toward nearest face
//}