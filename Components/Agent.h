#pragma once
#include "../Components/Transform.h"
#include "../Interfaces/IUpdatable.h"
#include "../MainComponents/Input.h"
#include "../Helper/GlobalVar.h"
#include <DirectXMath.h>
class Agent : public IUpdatable
{
protected:
	float mass, maxForce, maxSpeed;
	std::shared_ptr<Transform> transform;
	DirectX::XMFLOAT3 velocity;
	DirectX::XMFLOAT3 targetLook;
	DirectX::XMFLOAT3 currentLook;
	DirectX::XMFLOAT3 steeringForce;

public:
	Agent(float mass, float maxForce, float maxSpeed, std::shared_ptr<Transform> transform);

	void Update(float deltaTime) override;
	
	// Steering methods
	void Seek(DirectX::XMFLOAT3 target);
	void Flee(DirectX::XMFLOAT3 target);
	void Pursuit(DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 targetVelocity);
	void Evade(DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 targetVelocity);
	void Arrival(DirectX::XMFLOAT3 target, float slowingDistance);
};

