#include "Camera.h"

using namespace DirectX;
using namespace std;

Camera::Camera(DirectX::XMFLOAT3 pos, float movementSpeed, float mouseLookSpeed, float FOV, float aspectRatio, float nearClipPlane, float farClipPlane)
	: movementSpeed(movementSpeed), mouseLookSpeed(mouseLookSpeed), FOV(FOV), aspectRatio(aspectRatio), nearClipPlane(nearClipPlane), farClipPlane(farClipPlane)
{
	transform = std::make_shared<Transform>();
	transform->SetPosition(pos);
}

Camera::~Camera()
{
}

void Camera::Update(float dt)
{
	if (Input::KeyDown('W')) transform->MoveRelative(0, 0, movementSpeed);
	if (Input::KeyDown('S')) transform->MoveRelative(0, 0, -movementSpeed);
	if (Input::KeyDown('A')) transform->MoveRelative(-movementSpeed, 0, 0);
	if (Input::KeyDown('D')) transform->MoveRelative(movementSpeed, 0, 0);
	if (Input::KeyDown(' ')) transform->MoveRelative(0, movementSpeed, 0);
	if (Input::KeyDown('-')) transform->MoveRelative(0, -movementSpeed, 0);

	if (Input::MouseLeftDown()) {
		float xRot = Input::GetMouseXDelta() * mouseLookSpeed;
		float yRot = Input::GetMouseYDelta() * mouseLookSpeed;

		transform->Rotate(yRot, xRot, 0);
	}

	XMFLOAT3 rot = transform->GetPitchYawRoll();

	if (rot.x < XM_PIDIV2) rot.x = XM_PIDIV2 - 0.0001f;
	if (rot.x > XM_PIDIV2) rot.x = -XM_PIDIV2 + 0.0001f;

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	XMFLOAT3 pos = transform->GetPosition();
	XMFLOAT3 fwd = transform->GetForward();
	XMFLOAT3 worldUp = XMFLOAT3(0, 1, 0);

	XMStoreFloat4x4(&viewMatrix,
		XMMatrixLookToLH(
			XMLoadFloat3(&pos),
			XMLoadFloat3(&fwd),
			XMLoadFloat3(&worldUp)));
}

void Camera::UpdateProjectionMatrix(float aspectRatio)
{
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(FOV / XM_1DIVPI, aspectRatio, nearClipPlane, farClipPlane));
}

DirectX::XMFLOAT4X4 Camera::GetView() { return viewMatrix; }
DirectX::XMFLOAT4X4 Camera::GetProjection() { return projMatrix; }
std::shared_ptr<Transform> Camera::GetTransform() { return transform; }
