#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "PhysicsRenderer.h"
#include "Window.h"
#include <chrono>
#include <cstdlib>
#include <glm/glm.hpp>
#include <iostream>
#include <memory>
#include <string>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/matrix_clip_space.hpp>


//TODO consistent error handling
Window::Window(const int width, const int height, const std::string title)
	: m_width(width), m_height(height), m_windowTitle(title)
{
	// Create GLFW window object.  Renderer has to be initialized after this due to GLEW and GLFW global state.
	if (!createWindow(m_width, m_height)) exit(EXIT_FAILURE);

	// Setup callbacks.
	setupCallbacks();

	// View Matrix:
	m_view = glm::lookAt(m_eyePos, m_lookAtPoint, m_upVector);
}

bool Window::createWindow(const int width, const int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return 0;
	}

	// 4x antialiasing.
	glfwWindowHint(GLFW_SAMPLES, 4);

#ifdef __APPLE__ 
	// Apple implements its own version of OpenGL and requires special treatments
	// to make it uses modern OpenGL.

	// Ensure that minimum OpenGL version is 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	// Enable forward compatibility and allow a modern OpenGL context
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

	// Create the GLFW window, set Window class as user for callbacks
	m_glfwWindow = glfwCreateWindow(width, height, m_windowTitle.c_str(), NULL, NULL);
	glfwSetWindowUserPointer(m_glfwWindow, static_cast<void*>(this));

	// Check if the window could not be created.
	if (!m_glfwWindow)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return 0;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(m_glfwWindow);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return 0;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(m_glfwWindow, width, height);

	return 1;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window* windowObj = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!windowObj) exit(EXIT_FAILURE);
	windowObj->setDimensions(width, height);

	// Set the projection matrix.
	windowObj->m_projection = glm::perspective(glm::radians(60.0), 
								static_cast<double>(width) / 
								static_cast<double>(height), 1.0, 1000.0);
}

void Window::update() const
{
	// Perform updates. Multiple updates per frame helps with stability
	// TODO: Timestep should be 
	for (int i = 0; i < m_updatesPerFrame; ++i) {
		curRenderer->update(TIMESTEP);
	}
	
}

void Window::display()
{	
	// Resize viewport
	if (m_resizeFlag == true){
		curRenderer->setViewDimensions(m_width, m_height);
		m_resizeFlag = false;
	}
	
	// Render the objects
	curRenderer->draw(m_view, m_projection);


	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();

	// Swap buffers.
	glfwSwapBuffers(m_glfwWindow);
}

void Window::setDimensions(int width, int height)
{
	// update dims and set flag to resize window on next display() call
	m_height = height;
	m_width = width;
	m_resizeFlag = true;
}

void Window::keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// Check for a key press.
	if (action == GLFW_PRESS)
	{
		switch (key)
		{
		case GLFW_KEY_ESCAPE:
			// Close the window. This causes the program to also terminate.
			glfwSetWindowShouldClose(window, GL_TRUE);				
			break;
		default:
			break;
		}
	}
}

void Window::errorCallback(int error, const char* description)
{
	// Print error.
	std::cerr << description << std::endl;
}

void Window::setupCallbacks() const
{
	// Set the error callback.
	glfwSetErrorCallback(errorCallback);

	// Set the window resize callback.
	glfwSetWindowSizeCallback(m_glfwWindow, Window::resizeCallback);

	// Set the key callback.
	glfwSetKeyCallback(m_glfwWindow, Window::keyCallback);
}

void Window::displayLoop()
{
	if (!curRenderer) exit(EXIT_FAILURE);

	auto t0 = std::chrono::high_resolution_clock::now();

	// Loop while GLFW window should stay open.
	while (!glfwWindowShouldClose(m_glfwWindow))
	{
		if (m_enableTiming) {
			// Times display callback, prints to cout every 500 ms.
			auto t2 = std::chrono::high_resolution_clock::now();

			// Call renderer draw
			display();
			// Idle callback. Updating objects, etc. can be done here. (Update)
			update();

			if (std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t0).count() > 500.f) {
				auto t1 = std::chrono::high_resolution_clock::now();
				t0 = t1;
				std::cout << "Display and update callbacks took "
					<< std::chrono::duration_cast<std::chrono::microseconds>(t1 - t2).count()
					<< "microseconds\n";
			}

		}
		else {
			// Call renderer draw
			display();
			// Idle callback. Updating objects, etc. can be done here. (Update)
			update();
		}


	}
}

void Window::setRenderer(std::unique_ptr<IRenderer>&& renderer)
{
	curRenderer = std::move(renderer);
}

Window::~Window() {
	// Destroy the window.
	glfwDestroyWindow(m_glfwWindow);
	// Terminate GLFW.
	glfwTerminate();
}
