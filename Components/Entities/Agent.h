#pragma once
#include "../../Components/Transform.h"
#include "../../Interfaces/IUpdatable.h"
#include "../../MainComponents/Input.h"
#include "../../Helper/GlobalVar.h"

#include "PhysicsObject.h"
#include <DirectXMath.h>

class Agent : public PhysicsObject
{
protected:
	float maxForce;
	DirectX::XMFLOAT3 targetLook;
	DirectX::XMFLOAT3 currentLook;

public:
	Agent(float maxForce, PhysicsData data);

	void Update(float deltaTime) override;
	
	// Steering methods
	void Seek(DirectX::XMFLOAT3 target);
	void Flee(DirectX::XMFLOAT3 target);
	void Pursuit(DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 targetVelocity);
	void Evade(DirectX::XMFLOAT3 target, DirectX::XMFLOAT3 targetVelocity);
	void Arrival(DirectX::XMFLOAT3 target, float slowingDistance);

	void DrawImGui();
};

