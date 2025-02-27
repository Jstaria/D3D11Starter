#pragma once

#include <memory>

#include "DirectXMath.h"

#include "../Components/Mesh.h"
#include "../Components/Transform.h"

struct Particle {
	std::shared_ptr<Transform> transform;
	
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT4 appliedForceMass; // XYZ => Force | W => Mass
	DirectX::XMFLOAT4 translationalVelocityFriction; // XYZ => Velocity | W => Frictional Force
	DirectX::XMFLOAT4 rotationalVelocity; // XYZ => Velocity | W => Frictional Force
	DirectX::XMFLOAT4 scalarVelocity; // XYZ => Velocity | W => Frictional Force

	Particle(std::shared_ptr<Transform> parentTransform) 
	  : colorTint(1.0f, 1.0f, 1.0f, 1.0f), appliedForceMass(0.0f, 0.0f, 0.0f, 1.0f),
		translationalVelocityFriction(0.0f, 0.0f, 0.0f, 0.05f),
		rotationalVelocity(0.0f, 0.0f, 0.0f, 0.05f), scalarVelocity(0.0f, 0.0f, 0.0f, 0.05f)
	{
		this->transform = std::make_shared<Transform>();
		this->transform->SetParentTransform(parentTransform);
	}
};