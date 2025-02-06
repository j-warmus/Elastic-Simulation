#include "Window.h"

Window::Window() {
	// Todo Demagic number
	glfwWindow = createWindow(640, 480);
	if (!glfwWindow) exit(EXIT_FAILURE);

	// Setup callbacks.
	setupCallbacks(glfwWindow);

	windowTitle = std::string("GLFW Starter Project");

	// Todo figure out better way to error handle here
	if (!initializeProgram())
		exit(EXIT_FAILURE);

	if (!initializeObjects())
		exit(EXIT_FAILURE);

	// View Matrix:
	eyePos		= glm::vec3{ 0, 0, 20 };		// Camera position.
	lookAtPoint = glm::vec3{ 0, 0, 0 };		// The point we are looking at.
	upVector	= glm::vec3{ 0, 1, 0 };		// The up direction of the camera.
	view		= glm::lookAt(Window::eyePos, Window::lookAtPoint, Window::upVector);


}

bool Window::initializeProgram() {
	// Create a shader program with a vertex shader and a fragment shader.
	shaderProgram = LoadShaders("shaders/shader.vert", "shaders/shader.frag");

	// Check the shader program.
	if (!shaderProgram)
	{
		std::cerr << "Failed to initialize shader program" << std::endl;
		return false;
	}

	return true;
}

bool Window::initializeObjects()
{
	curRenderable = std::make_unique<ElasticManager>();

	return true;
}

void Window::cleanUp() const
{
	// Delete the shader program.
	glDeleteProgram(shaderProgram);
}

GLFWwindow* Window::createWindow(int width, int height)
{
	// Initialize GLFW.
	if (!glfwInit())
	{
		std::cerr << "Failed to initialize GLFW" << std::endl;
		return NULL;
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
	GLFWwindow* window = glfwCreateWindow(width, height, windowTitle.c_str(), NULL, NULL);
	glfwSetWindowUserPointer(window, static_cast<void*>(this));

	// Check if the window could not be created.
	if (!window)
	{
		std::cerr << "Failed to open GLFW window." << std::endl;
		glfwTerminate();
		return NULL;
	}

	// Make the context of the window.
	glfwMakeContextCurrent(window);

#ifndef __APPLE__
	// On Windows and Linux, we need GLEW to provide modern OpenGL functionality.

	// Initialize GLEW.
	if (glewInit())
	{
		std::cerr << "Failed to initialize GLEW" << std::endl;
		return NULL;
	}
#endif

	// Set swap interval to 1.
	glfwSwapInterval(0);

	// Call the resize callback to make sure things get drawn immediately.
	Window::resizeCallback(window, width, height);

	return window;
}

void Window::resizeCallback(GLFWwindow* window, int width, int height)
{
#ifdef __APPLE__
	// In case your Mac has a retina display.
	glfwGetFramebufferSize(window, &width, &height); 
#endif
	Window* windowObj = static_cast<Window*>(glfwGetWindowUserPointer(window));
	if (!windowObj) exit(EXIT_FAILURE);
	windowObj->width = width;
	windowObj->height = height;
	// Set the viewport size.
	glViewport(0, 0, width, height);

	// Set the projection matrix.
	windowObj->projection = glm::perspective(glm::radians(60.0), 
								static_cast<double>(width) / 
								static_cast<double>(height), 1.0, 1000.0);
}

void Window::idleCallback() const
{
	// TODO: make this actually run at a stable framerate
	// Perform any necessary updates here 
	for (int i = 0; i < 300; i++) {
		curRenderable->update();
	}
}

void Window::displayCallback(GLFWwindow* window) const
{	
	// Clear the color and depth buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);	

	// Render the objects
	curRenderable->draw(view, projection, shaderProgram);

	// Gets events, including input such as keyboard and mouse or window resizing
	glfwPollEvents();

	// Swap buffers.
	glfwSwapBuffers(window);
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

void Window::setupCallbacks(GLFWwindow* window) const
{
	// Set the error callback.
	glfwSetErrorCallback(errorCallback);

	// Set the window resize callback.
	glfwSetWindowSizeCallback(window, Window::resizeCallback);

	// Set the key callback.
	glfwSetKeyCallback(window, Window::keyCallback);
}

void Window::displayLoop() const
{
	// Loop while GLFW window should stay open.
	while (!glfwWindowShouldClose(glfwWindow))
	{
		// Main render display callback. Rendering of objects is done here. (Draw)
		displayCallback(glfwWindow);

		// Idle callback. Updating objects, etc. can be done here. (Update)
		idleCallback();
	}
}

Window::~Window() {
	cleanUp();
	// Destroy the window.
	glfwDestroyWindow(glfwWindow);
	// Terminate GLFW.
	glfwTerminate();
}
