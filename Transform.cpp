#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1),
	dirty(true)
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
}

void Transform::SetScale(float x, float y, float z)
{
	XMStoreFloat3(&scale, XMVectorSet(x, y, z, 0.0f));
	NotifyOfCleanliness(true);
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
	dirty = value;
}

DirectX::XMFLOAT3 Transform::GetPosition()
{
	return position;
}

XMFLOAT3 Transform::GetPitchYawRoll()
{
	return rotation;
}

XMFLOAT3 Transform::GetScale()
{
	return scale;
}

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (dirty) {
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

		dirty = false;
	}

	return worldMatrix;
}

XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return worldInverseTransposeMatrix;
}

std::shared_ptr<Transform> Transform::GetParentTransform()
{
	return parentTransform;
}

bool Transform::GetDirty()
{
	return dirty;
}

void Transform::MoveAbsolute(float x, float y, float z)
{
	XMStoreFloat3(&position, XMVECTOR(XMLoadFloat3(&position)) + XMVectorSet(x, y, z, 0.0f));
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	XMStoreFloat3(&position, XMVECTOR(XMLoadFloat3(&position)) + XMVECTOR(XMLoadFloat3(&offset)));
}

void Transform::MoveRelative(float x, float y, float z)
{
}

void Transform::MoveRelative(DirectX::XMFLOAT3 offset)
{
}

void Transform::Rotate(float p, float y, float r)
{
	XMStoreFloat3(&rotation, XMVectorSet(rotation.x + p, rotation.y + y, rotation.z + r, 0.0f));
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation)
{
	XMStoreFloat3(&this->rotation, XMVECTOR(XMLoadFloat3(&this->rotation)) + XMVECTOR(XMLoadFloat3(&rotation)));
}

void Transform::Scale(float x, float y, float z)
{
	XMStoreFloat3(&scale, XMVectorSet(scale.x * x, scale.y * y, scale.z * z, 0.0f));
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	XMStoreFloat3(&scale, XMVectorSet(this->scale.x * scale.x, this->scale.y * scale.y, this->scale.z * scale.z, 0.0f));
}

void Transform::NotifyOfCleanliness(bool success) {
	dirty = true;

	for (const auto& child : childTransforms) {
		child->OnClean(success);
	}
}

void Transform::OnClean(bool success)
{
	dirty = success;
}
