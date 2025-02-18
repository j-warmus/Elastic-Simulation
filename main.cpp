#include "main.h"
#include "PhysicsRenderer.h"
#include "Window.h"
#include <cstdlib>
#include <memory>
#include "Renderer.h"
#include <string>
#include <type_traits>

int main(void)
{
	// Create the GLFW window.
	
	Window window(1200, 1200, "OpenGL Elastic Physics Demo");

	std::unique_ptr<PhysicsRenderer> renderer{ std::make_unique<PhysicsRenderer>() };

	window.setRenderer(std::move(renderer));

	window.displayLoop();

	exit(EXIT_SUCCESS);
}