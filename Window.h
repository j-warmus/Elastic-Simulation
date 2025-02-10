#ifndef _WINDOW_H_
#define _WINDOW_H_

#include "main.h"
#include "shader.h"
#include "Renderer.h"
#include "PhysicsRenderer.h"
#include <memory>

class Window
{
public:
	// Renderer Object
	std::unique_ptr<Renderer> curRenderer;

	// Window Properties
	int m_width;
	int m_height;
	const std::string m_windowTitle;


	// Camera Matrices
	// TODO these should probably all be part of the renderer?
	glm::mat4 m_projection;
	glm::mat4 m_view;
	glm::vec3 m_eyePos		= glm::vec3{0, 0, 20};
	glm::vec3 m_lookAtPoint	= glm::vec3{0, 0, 0};
	glm::vec3 m_upVector	= glm::vec3{0, 1, 0};;

	// Shader Program ID
	GLuint shaderProgram;

	// Main loop
	void displayLoop() const;

	// Constructors and Destructors
	Window();
	Window(const int width, const int height, const std::string& title);
	// Todo impelment Window(eyepos, lookat, up, const int width, const int height, )
	~Window();
	bool initializeProgram();
	bool initializeObjects();
	void cleanUp() const;

	// Generates m_glfwWindow, returns error code
	bool createWindow(int width, int height);

	// Draw and Update renderer
	void update() const;
	void display() const;

	// GLFW callbacks, have to be static to be used by GLFW
	static void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
	static void errorCallback(int error, const char* description);
	static void resizeCallback(GLFWwindow* window, int width, int height);

private:
	// Raw pointer necessary due to opaque GLFWwindow struct
	GLFWwindow* m_glfwWindow;
	void setupCallbacks() const;
	
};

#endif
