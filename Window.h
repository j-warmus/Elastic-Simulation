#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "main.h"
#include "shader.h"
#include "Object.h"
#include "Manager.h"
#include <memory>

class Window
{
public:
	// Renderable Object
	std::unique_ptr<Renderable> curRenderable;

	// Window Properties
	int width;
	int height;
	std::string windowTitle;


	// Camera Matrices
	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 eyePos, lookAtPoint, upVector;

	// Shader Program ID
	GLuint shaderProgram;

	// Main loop
	void Window::displayLoop() const;

	// Constructors and Destructors
	Window();
	~Window();
	bool initializeProgram();
	bool initializeObjects();
	void cleanUp() const;

	// Window functions
	GLFWwindow* createWindow(int width, int height);

	// Draw and Update functions
	void idleCallback() const;
	void displayCallback(GLFWwindow*) const;

	// Callbacks, have to be static to be used by GLFW
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void errorCallback(int error, const char* description);
	static void resizeCallback(GLFWwindow* window, int width, int height);

private:
	GLFWwindow* glfwWindow;
	void setupCallbacks(GLFWwindow* window) const;
	
};

#endif
