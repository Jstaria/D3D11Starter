#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1),
	matricesDirty(true),
	up(XMFLOAT3(0,1,0)),
	right(XMFLOAT3(1, 0, 0)),
	forward(XMFLOAT3(0, 0, 1))
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixIdentity());
	XMStoreFloat4(&quaternion, XMQuaternionIdentity());
}

Transform::~Transform()
{
	
}

void Transform::SetPosition(float x, float y, float z)
{
	XMStoreFloat3(&position, XMVectorSet(x, y, z, 0.0f));
	NotifyOfCleanliness(true);
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	XMStoreFloat3(&this->position, XMVECTOR(XMLoadFloat3(&position)));
	NotifyOfCleanliness(true);
}

void Transform::SetRotation(float p, float y, float r)
{
	XMStoreFloat3(&rotation, XMVectorSet(p, y, r, 0.0f));
	NotifyOfCleanliness(true);
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	XMStoreFloat3(&this->rotation, XMVECTOR(XMLoadFloat3(&rotation)));
	NotifyOfCleanliness(true);
	UpdateVectors();
}

void Transform::SetScale(float x, float y, float z)
{
	XMStoreFloat3(&scale, XMVectorSet(x, y, z, 0.0f));
	NotifyOfCleanliness(true);
	UpdateVectors();
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	XMStoreFloat3(&this->scale, XMVECTOR(XMLoadFloat3(&scale)));
	NotifyOfCleanliness(true);
}
void Transform::SetParentTransform(std::shared_ptr<Transform> transform)
{
	parentTransform = transform;
	
	if (transform != nullptr)
		parentTransform.get()->SetChildTransform(this);
}

void Transform::SetChildTransform(Transform* transform)
{
	childTransforms.push_back(transform);
}

void Transform::SetDirty(bool value)
{
	matricesDirty = value;
}

DirectX::XMFLOAT3 Transform::GetPosition() { return position; }
XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }
XMFLOAT3 Transform::GetScale() { return scale; }

DirectX::XMFLOAT3 Transform::GetUp() { return up; }
DirectX::XMFLOAT3 Transform::GetRight() { return right; }
DirectX::XMFLOAT3 Transform::GetForward() { return forward; }

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (matricesDirty) {
		XMStoreFloat4(&quaternion, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));

		XMMATRIX trMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion));
		XMMATRIX scMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
		XMMATRIX parentMatrix = XMMatrixIdentity();

		if (parentTransform != nullptr) {
			XMFLOAT4X4 parent = parentTransform.get()->GetWorldMatrix();
			parentMatrix = XMLoadFloat4x4(&parent);
		}

		//XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(scMatrix, rotMatrix), trMatrix); // No aspect ratio correction
		XMMATRIX world = XMMatrixMultiply(XMMatrixMultiply(XMMatrixMultiply(scMatrix, rotMatrix), trMatrix), parentMatrix);

		XMStoreFloat4x4(&worldMatrix, world);
		XMStoreFloat4x4(&worldInverseTransposeMatrix, XMMatrixInverse(0, XMMatrixTranspose(world)));

		matricesDirty = false;
	}

	return worldMatrix;
}

XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix() { return worldInverseTransposeMatrix; }

std::shared_ptr<Transform> Transform::GetParentTransform() { return parentTransform; }

bool Transform::GetDirty() { return matricesDirty; }

void Transform::MoveAbsolute(float x, float y, float z)
{
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMVectorSet(x, y, z, 0.0f));
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	XMStoreFloat3(&position, XMLoadFloat3(&position) + XMLoadFloat3(&offset));
}

void Transform::MoveRelative(float x, float y, float z)
{
	XMVECTOR movement = XMVectorSet(x, y, z, 0);
	XMVECTOR rotQuat = XMLoadFloat4(&quaternion);

	XMVECTOR dir = XMVector3Rotate(movement, rotQuat);

	XMStoreFloat3(&position, XMVECTOR(XMLoadFloat3(&position)) + dir);

	NotifyOfCleanliness(true);
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
	MoveRelative(offset.x, offset.y, offset.z);
}

void Transform::Rotate(float p, float y, float r)
{
	XMStoreFloat3(&rotation, XMVectorSet(rotation.x + p, rotation.y + y, rotation.z + r, 0.0f));
	UpdateVectors();
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	XMStoreFloat3(&this->rotation, XMLoadFloat3(&this->rotation) + XMLoadFloat3(&rotation));
	UpdateVectors();
}

void Transform::Scale(float x, float y, float z)
{
	XMStoreFloat3(&scale, XMVectorSet(scale.x * x, scale.y * y, scale.z * z, 0.0f));
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	XMStoreFloat3(&this->scale, XMVectorSet(this->scale.x * scale.x, this->scale.y * scale.y, this->scale.z * scale.z, 0.0f));
}

void Transform::NotifyOfCleanliness(bool success) {
	matricesDirty = true;

	for (const auto& child : childTransforms) {
		child->OnClean(success);
	}
}

void Transform::OnClean(bool success)
{
	matricesDirty = success;
}

void Transform::UpdateVectors()
{
	XMStoreFloat4(&quaternion, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));
	XMVECTOR rotQuat = XMLoadFloat4(&quaternion);

	XMStoreFloat3(&up, XMVector3Rotate(XMLoadFloat3(&up), rotQuat));
	XMStoreFloat3(&right, XMVector3Rotate(XMLoadFloat3(&right), rotQuat));
	XMStoreFloat3(&forward, XMVector3Rotate(XMLoadFloat3(&forward), rotQuat));
}
