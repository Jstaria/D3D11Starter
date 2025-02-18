#include "Camera.h"

using namespace DirectX;
using namespace std;

Camera::Camera(const char* name, DirectX::XMFLOAT3 pos, float movementSpeed, float mouseLookSpeed, float FOV, float aspectRatio, float nearClipPlane, float farClipPlane)
	: name(name), movementSpeed(movementSpeed), mouseLookSpeed(mouseLookSpeed), FOV(FOV), aspectRatio(aspectRatio), nearClipPlane(nearClipPlane), farClipPlane(farClipPlane)
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
	float speed = movementSpeed * dt;

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
		
		transform->Rotate(yRot, xRot, 0);

		XMFLOAT3 rot = transform->GetPitchYawRoll();
		if (rot.x > XM_PIDIV2) rot.x = XM_PIDIV2 - 0.0000001f;
		if (rot.x < -XM_PIDIV2) rot.x = -XM_PIDIV2 + 0.0000001f;
		transform->SetRotation(rot);
	}

	if (Input::MouseLeftDown()) {
		transform->SetRotation(0,0,0);
	}

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

void Camera::UIDraw()
{
	if (ImGui::TreeNode(name)) {

		if (parentObj != nullptr)
			ImGui::Text("Parent GameObject: %s", parentObj.get()->GetName());

		{
			XMFLOAT3 pos = transform.get()->GetPosition();
			ImGui::DragFloat3(("Position##" + std::string(name)).c_str(), &pos.x, .01f);
			transform.get()->SetPosition(pos);
		}

		{
			XMFLOAT3 rot = transform.get()->GetPitchYawRoll();
			ImGui::DragFloat3(("Rotation##" + std::string(name)).c_str(), &rot.x, .01f);
			transform.get()->SetRotation(rot);
		}

		{
			XMFLOAT3 scale = transform.get()->GetScale();
			ImGui::DragFloat3(("Scale##" + std::string(name)).c_str(), &scale.x, .01f);
			transform.get()->SetScale(scale);
		}

		ImGui::TreePop();
	}
}

