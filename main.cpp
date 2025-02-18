#include "ElasticEngine.h"
#include "OpenGlBackend.h"
#include "PhysicsRenderer.h"
#include "Window.h"
#include <cstdlib>
#include <memory>
#include <type_traits>

int main(void)
{
	// Create the GLFW window. This has to be done first as it also initializes the OpenGl context
	Window window(1200, 1200, "OpenGL Elastic Physics Demo");

	// Set simulation parameters here
	elasticParams elasticParameters;
	cubeParams cubeParameters;

	// Physics Engine Setup
	std::unique_ptr<ElasticEngine> engine{ std::make_unique<ElasticEngine>(elasticParameters, cubeParameters) };
	engine->generateCubeGeometry();

	// Render Backend Setup
	std::unique_ptr<OpenGlBackend> backend{ std::make_unique<OpenGlBackend>() };
	backend->initializeShadersFromFile("shaders/shader.vert", "shaders/shader.frag");

	// Compose into Physics Renderer
	std::unique_ptr<PhysicsRenderer> renderer{ std::make_unique<PhysicsRenderer>(std::move(engine), std::move(backend)) };

	// Attach Renderer to Window and display/run simulation
	window.setRenderer(std::move(renderer));

	window.displayLoop();

	exit(EXIT_SUCCESS);
}