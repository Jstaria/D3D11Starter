#include "Agent.h"

using namespace DirectX;
using namespace std;

Agent::Agent(float maxForce, PhysicsData data)
	: PhysicsObject(data), maxForce(maxForce)
{
}

void Agent::Update(float deltaTime)
{
	//printf("{%.2f, %.2f, %.2f}\n", velocity.x, velocity.y, velocity.z);

	PhysicsObject::Update(deltaTime);

	transform->LookAtRelative(velocity);
}

void Agent::Flee(DirectX::XMFLOAT3 target)
{
	XMFLOAT3 position = transform->GetPosition();

	XMVECTOR targetVec = XMLoadFloat3(&target);
	XMVECTOR positionVec = XMLoadFloat3(&position);
	XMVECTOR desiredVel = XMVector3Normalize(positionVec - targetVec) * maxSpeed;

	XMVECTOR velocityVec = XMLoadFloat3(&velocity);

	XMFLOAT3 force;

	XMStoreFloat3(&force, desiredVel - velocityVec);

	ApplyForce(force);
}

void Agent::Seek(DirectX::XMFLOAT3 target)
{
	XMFLOAT3 position = transform->GetPosition();

	XMVECTOR targetVec = XMLoadFloat3(&target);
	XMVECTOR positionVec = XMLoadFloat3(&position);
	XMVECTOR desiredVel = XMVector3Normalize(targetVec - positionVec) * maxSpeed;

	XMVECTOR velocityVec = XMLoadFloat3(&velocity);

	XMFLOAT3 force;

	XMStoreFloat3(&force, desiredVel - velocityVec);

	ApplyForce(force);
}

void Agent::Pursuit(DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 targetVelocity)
{
	DirectX::XMFLOAT3 targetPosition{ 
		target.x + targetVelocity.x, 
		target.y + targetVelocity.y, 
		target.z + targetVelocity.z };

	Seek(targetPosition);
}

void Agent::Evade(DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 targetVelocity)
{
	DirectX::XMFLOAT3 targetPosition{
	target.x + targetVelocity.x,
	target.y + targetVelocity.y,
	target.z + targetVelocity.z };

	Flee(targetPosition);
}

void Agent::Arrival(DirectX::XMFLOAT3 target, float slowingDistance)
{
	XMFLOAT3 position = transform->GetPosition();

	XMVECTOR targetVec = XMLoadFloat3(&target);
	XMVECTOR positionVec = XMLoadFloat3(&position);

	XMVECTOR toTarget = targetVec - positionVec;
	XMVECTOR distanceVec = XMVector3Length(toTarget);
	float distance = XMVectorGetX(distanceVec);

	if (distance < FLT_EPSILON) 
		return;

	float rampedSpeed = maxSpeed * (distance / max(slowingDistance, FLT_EPSILON));
	float clippedSpeed = min(rampedSpeed, maxSpeed);

	XMVECTOR desiredVel = toTarget * (clippedSpeed / distance);

	XMVECTOR steeringVec = desiredVel - XMLoadFloat3(&velocity);
	XMFLOAT3 force;

	XMStoreFloat3(&force, steeringVec);

	ApplyForce(force);
}

void Agent::DrawImGui()
{
	ImGui::SliderFloat("Mass", &mass, .01f, 100.0f);
	ImGui::SliderFloat("Max Speed", &maxSpeed, .01f, 100.0f);
	ImGui::SliderFloat("Max Force", &maxForce, .01f, 100.0f);
	ImGui::SliderFloat("Friction Coefficient", &frictionCoeff, .01f, 5.0f);
	ImGui::Checkbox("Use Gravity", &useGravity);
	ImGui::Checkbox("Use Friction", &useFriction);
}
