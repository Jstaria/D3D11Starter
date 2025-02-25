#include "Camera.h"

using namespace DirectX;
using namespace std;

Camera::Camera(const char* name, DirectX::XMFLOAT3 pos, float FOV, float aspectRatio, float nearClipPlane, float farClipPlane)
	: name(name), FOV(FOV), aspectRatio(aspectRatio), nearClipPlane(nearClipPlane), farClipPlane(farClipPlane)
{
	transform = std::make_shared<Transform>();
	transform->SetPosition(pos);

	UpdateViewMatrix();
	UpdateProjectionMatrix(aspectRatio);
}

Camera::~Camera()
{
}

void Camera::Update(float dt)
{
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
	XMStoreFloat4x4(&projMatrix, XMMatrixPerspectiveFovLH(FOV * (XM_PI / 180), aspectRatio, nearClipPlane, farClipPlane));
}

DirectX::XMFLOAT4X4 Camera::GetView() { return viewMatrix; }
DirectX::XMFLOAT4X4 Camera::GetProjection() { return projMatrix; }
std::shared_ptr<Transform> Camera::GetTransform() { return transform; }

const char* Camera::GetName()
{
	return name;
}

void Camera::UIDraw()
{
	ImGui::Text(name);

	if (parentObj != nullptr)
		ImGui::Text("Parent GameObject: %s", parentObj->GetName());

	{
		XMFLOAT3 pos = transform->GetPosition();
		ImGui::DragFloat3(("Position##" + std::string(name)).c_str(), &pos.x, .01f);
		transform->SetPosition(pos);
	}

	{
		XMFLOAT3 rot = transform->GetPitchYawRoll();
		ImGui::DragFloat3(("Rotation##" + std::string(name)).c_str(), &rot.x, .01f);
		transform->SetRotation(rot, Angle::PI);
	}

	{
		XMFLOAT3 scale = transform->GetScale();
		ImGui::DragFloat3(("Scale##" + std::string(name)).c_str(), &scale.x, .01f);
		transform->SetScale(scale);
	}
}

