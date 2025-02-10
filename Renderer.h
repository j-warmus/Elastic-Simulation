#ifndef _OBJECT_H_
#define _OBJECT_H_

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#else
#include <GL/glew.h>
#endif

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Renderer
{
public:
	virtual void draw(const glm::mat4& view, const glm::mat4& projection, GLuint shader) = 0;
	virtual void update() = 0;
	virtual ~Renderer() {};
};

#endif

