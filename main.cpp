#include "main.h"
#include "Window.h"
#include <cstdlib>
#include <memory>

int main(void)
{
	// Create the GLFW window.
	std::unique_ptr<Window> window = std::make_unique<Window>();

	window->displayLoop();

	exit(EXIT_SUCCESS);
}