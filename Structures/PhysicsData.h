#pragma once
#include "../Components/Transform.h"
#include <DirectXMath.h>

struct PhysicsData {
	float mass;
	float maxSpeed;
	float radius;
	float frictionCoeff;
	std::shared_ptr<Transform> transform;
	bool useGravity;
	bool useFriction;
};