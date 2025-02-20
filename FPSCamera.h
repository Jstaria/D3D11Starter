#pragma once
#include "Camera.h"
#include "DirectXMath.h"
#include "Transform.h"

class FPSCamera : public Camera
{
private:
	float movementSpeed;
	float mouseLookSpeed;
public:
	FPSCamera(const char* name, DirectX::XMFLOAT3 pos, float movementSpeed, float mouseLookSpeed, float FOV, float aspectRatio, float nearClipPlane, float farClipPlane);
	void Update(float deltaTime) override;

};

