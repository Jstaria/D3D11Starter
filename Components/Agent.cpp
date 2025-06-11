#include "Agent.h"

using namespace DirectX;
using namespace std;

Agent::Agent(float mass, float maxForce, float maxSpeed, std::shared_ptr<Transform> transform)
	: mass(mass), maxForce(maxForce), maxSpeed(maxSpeed), 
	transform(transform), steeringForce({0,0,0}), 
	velocity({0,0,0})
{
}

void Agent::Update(float deltaTime)
{
	if (steeringForce.x + steeringForce.y + steeringForce.z == 0)
		return;

	XMVECTOR acceleration{ 0,0,0 };
	XMVECTOR steeringVec = XMVector3ClampLength(XMLoadFloat3(&steeringForce), 0.0f, maxForce);

	acceleration = steeringVec / mass;

	XMVECTOR velocityVec = XMLoadFloat3(&velocity);

	XMStoreFloat3(&velocity, XMVector3ClampLength((velocityVec + acceleration * deltaTime), 0.0f, maxSpeed));

	printf("{%.2f, %.2f, %.2f}\n", velocity.x, velocity.y, velocity.z);

	transform->MoveAbsolute(velocity);
	transform->LookAtRelative(velocity);
}

void Agent::Seek(DirectX::XMFLOAT3 target)
{
	XMFLOAT3 position = transform->GetPosition();

	XMVECTOR targetVec = XMLoadFloat3(&target);
	XMVECTOR positionVec = XMLoadFloat3(&position);
	XMVECTOR desiredVel = XMVector3Normalize(positionVec - targetVec) * maxSpeed;

	XMVECTOR velocityVec = XMLoadFloat3(&velocity);

	XMStoreFloat3(&steeringForce, desiredVel - velocityVec);
}

void Agent::Flee(DirectX::XMFLOAT3 target)
{
	XMFLOAT3 position = transform->GetPosition();

	XMVECTOR targetVec = XMLoadFloat3(&target);
	XMVECTOR positionVec = XMLoadFloat3(&position);
	XMVECTOR desiredVel = XMVector3Normalize(targetVec - positionVec) * maxSpeed;

	XMVECTOR velocityVec = XMLoadFloat3(&velocity);

	XMStoreFloat3(&steeringForce, desiredVel - velocityVec);
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
	XMStoreFloat3(&steeringForce, steeringVec);
}
