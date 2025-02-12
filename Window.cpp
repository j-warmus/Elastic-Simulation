#include "Window.h"


//TODO all error code needs look.
Window::Window() : Window(1200, 1200, "GLFW Window"){}

Window::Window(const int width, const int height, const std::string& title) 
	: m_width(width), m_height(height), m_windowTitle(title) 
{
	// Create GLFW window object
	if (!createWindow(m_width, m_height)) exit(EXIT_FAILURE);

	// Setup callbacks.
	setupCallbacks();

	curRenderer = std::make_unique<PhysicsRenderer>();

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
	// TODO: make this actually run at a stable framerate
	// Perform any necessary updates here 
	for (int i = 0; i < 300; i++) {
		curRenderer->update();
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
	curRenderer->draw(m_view, m_projection, shaderProgram);

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
	// Loop while GLFW window should stay open.
	while (!glfwWindowShouldClose(m_glfwWindow))
	{
		// Call renderer draw
		display();

		// Idle callback. Updating objects, etc. can be done here. (Update)
		update();
	}
}

Window::~Window() {
	// Destroy the window.
	glfwDestroyWindow(m_glfwWindow);
	// Terminate GLFW.
	glfwTerminate();
}
