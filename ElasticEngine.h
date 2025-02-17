#pragma once
#include "PhysicsEngine.h"
#include "main.h"
class ElasticEngine : public PhysicsEngine
{
public:
	void generateCubeGeometry(glm::vec3 startpos, int w, int h, int d);
	~ElasticEngine();

private:

};

