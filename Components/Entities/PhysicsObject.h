#pragma once
#include "../../Components/Transform.h"
#include "../../Interfaces/IUpdatable.h"
#include "../../Structures/PhysicsData.h"

class PhysicsObject : public IUpdatable
{
protected:
	float mass;
	float maxSpeed;
	float radius;
	float frictionCoeff;
	std::shared_ptr<Transform> transform;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 acceleration;
	DirectX::XMFLOAT3 friction;
	bool useGravity;
	bool useFriction;

	void ApplyGravity(DirectX::XMFLOAT3 force);
	void ApplyFriction(float coeff);
	DirectX::XMFLOAT3 CalculateFuturePosition(float time);

	// Setters
	void SetVelocity(DirectX::XMFLOAT3 velocity);
	void ApplyForce(DirectX::XMFLOAT3 force);

public:

	PhysicsObject(float mass, float maxSpeed, float radius, float frictionCoeff,
		std::shared_ptr<Transform> transform, bool useGravity, bool useFriction);
	PhysicsObject(PhysicsData data);

	// Getters
	float GetRadius();
	DirectX::XMFLOAT3 GetVelocity();
	std::shared_ptr<Transform> GetTransform();

	void Update(float deltaTime) override;
	void FixedUpdate(float deltaTime) override;
};

