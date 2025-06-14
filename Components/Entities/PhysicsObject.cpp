#include "PhysicsObject.h"

using namespace DirectX;
using namespace std;

PhysicsObject::PhysicsObject(float mass, float maxSpeed, float radius, float frictionCoeff, std::shared_ptr<Transform> transform, bool useGravity, bool useFriction)
	: mass(mass), maxSpeed(maxSpeed), radius(radius), frictionCoeff(frictionCoeff), transform(transform), useFriction(useFriction), useGravity(useGravity)
{
}

PhysicsObject::PhysicsObject(PhysicsData data)
	: mass(data.mass), maxSpeed(data.maxSpeed), radius(data.radius), frictionCoeff(data.frictionCoeff), transform(data.transform), useFriction(data.useFriction), useGravity(data.useGravity)
{
}

float PhysicsObject::GetRadius() { return radius; }
DirectX::XMFLOAT3 PhysicsObject::GetVelocity() { return velocity; }
std::shared_ptr<Transform> PhysicsObject::GetTransform() { return transform; }

void PhysicsObject::Update(float deltaTime)
{

}

void PhysicsObject::FixedUpdate(float deltaTime)
{
	if (useGravity) ApplyGravity({ 0,-9.81f,0 });
	if (useFriction) ApplyFriction(frictionCoeff);

	XMVECTOR velVec = XMLoadFloat3(&velocity);
	XMVECTOR accVec = XMLoadFloat3(&acceleration);

	velVec += XMVector3ClampLength(accVec * deltaTime, 0, maxSpeed);
	XMStoreFloat3(&velocity, velVec);

	transform->MoveAbsolute(velocity);

	acceleration = { 0,0,0 };
}

void PhysicsObject::ApplyGravity(DirectX::XMFLOAT3 force)
{
	acceleration.x += force.x;
	acceleration.y += force.y;
	acceleration.z += force.z;
}

void PhysicsObject::ApplyFriction(float coeff)
{
	XMVECTOR velVec = XMLoadFloat3(&velocity);
	XMVECTOR fricVec = XMVector3Normalize(-velVec) * coeff;
	XMStoreFloat3(&friction, fricVec);

	ApplyForce(friction);
}

DirectX::XMFLOAT3 PhysicsObject::CalculateFuturePosition(float time)
{
	XMFLOAT3 position = transform->GetPosition();

	XMVECTOR velVec = XMLoadFloat3(&velocity);
	XMVECTOR posVec = XMLoadFloat3(&position);

	XMStoreFloat3(&position, posVec + velVec * time);

	return position;
}

void PhysicsObject::SetVelocity(DirectX::XMFLOAT3 velocity) { this->velocity = velocity; }

void PhysicsObject::ApplyForce(DirectX::XMFLOAT3 force)
{
	acceleration.x += force.x / mass;
	acceleration.y += force.y / mass;
	acceleration.z += force.z / mass;
}
