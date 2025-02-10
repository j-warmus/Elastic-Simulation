#pragma once

class PhysicsEngine
{
	virtual void simulate(float timestep) = 0;
	virtual ~PhysicsEngine() {};
};

