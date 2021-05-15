#pragma once

#include "FParticle.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <GL/glew.h>
#include <memory>
#include <vector>
#include <random>
#include <unordered_map>

class SpatialTable
{
private:
	std::unordered_map<int, std::vector<int>*> table;
	
public:
	int m;
	float d;
	int p1, p2, p3;


	int hash(glm::vec3 position);
	std::vector<int>* locate(glm::vec3 pos);
	void build(std::vector<FParticle> particles);
	void clear();
	void getneighbors(FParticle particle, float support, std::vector<FParticle> particles);
};

