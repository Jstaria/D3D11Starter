#include "FPSCamera.h"

using namespace DirectX;
using namespace Microsoft::WRL;

FPSCamera::FPSCamera(const char* name, DirectX::XMFLOAT3 pos, float movementSpeed, float mouseLookSpeed, float FOV, float aspectRatio, float nearClipPlane, float farClipPlane)
	: Camera(name, pos, FOV, aspectRatio, nearClipPlane, farClipPlane),
	movementSpeed(movementSpeed), mouseLookSpeed(mouseLookSpeed)
{
}

void FPSCamera::Update(float deltaTime) 
{
	float speed = movementSpeed * deltaTime;

	if (Input::KeyDown(VK_CONTROL)) speed *= 2;

	if (Input::KeyDown('W')) transform->MoveRelative(0, 0, speed);
	if (Input::KeyDown('S')) transform->MoveRelative(0, 0, -speed);
	if (Input::KeyDown('A')) transform->MoveRelative(-speed, 0, 0);
	if (Input::KeyDown('D')) transform->MoveRelative(speed, 0, 0);
	if (Input::KeyDown(' ')) transform->MoveRelative(0, speed, 0);
	if (Input::KeyDown(VK_SHIFT)) transform->MoveRelative(0, -speed, 0);



	if (Input::MouseRightDown()) {

		float xRot = mouseLookSpeed * Input::GetMouseXDelta();
		float yRot = mouseLookSpeed * Input::GetMouseYDelta();
		//printf("{%.5f,%.5f}\n", xRot, yRot);
		//printf("{%d,%d}\n", Input::GetMouseXDelta(), Input::GetMouseYDelta());

		transform->Rotate(yRot, xRot, 0, Angle::RADIANS);

		XMFLOAT3 rot = transform->GetPitchYawRoll();
		if (rot.x > XM_PIDIV2) rot.x = XM_PIDIV2 - 0.0000001f;
		if (rot.x < -XM_PIDIV2) rot.x = -XM_PIDIV2 + 0.0000001f;
		transform->SetRotation(rot, Angle::RADIANS);
	}

	Camera::Update(deltaTime);
}
