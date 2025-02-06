#include "Transform.h"

using namespace DirectX;

Transform::Transform() :
	position(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1)
{
	XMStoreFloat4x4(&worldMatrix, XMMatrixIdentity());
}

void Transform::SetPosition(float x, float y, float z)
{
	XMStoreFloat3(&position, XMVectorSet(x, y, z, 0.0f));
	dirty = true;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	XMStoreFloat3(&this->position, XMVECTOR(XMLoadFloat3(&position)));
	dirty = true;
}

void Transform::SetRotation(float p, float y, float r)
{
	XMStoreFloat3(&rotation, XMVectorSet(p, y, r, 0.0f));
	dirty = true;
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation)
{
	XMStoreFloat3(&this->rotation, XMVECTOR(XMLoadFloat3(&rotation)));
	dirty = true;
}

void Transform::SetScale(float x, float y, float z)
{
	XMStoreFloat3(&scale, XMVectorSet(x, y, z, 0.0f));
	dirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	XMStoreFloat3(&this->scale, XMVECTOR(XMLoadFloat3(&scale)));
	dirty = true;
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
		XMMATRIX trMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
		XMMATRIX rotMatrix = XMMatrixRotationRollPitchYawFromVector(XMLoadFloat3(&rotation));
		XMMATRIX scMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale));

		XMStoreFloat4x4(&worldMatrix, XMMatrixMultiply(XMMatrixMultiply(scMatrix, rotMatrix), trMatrix));

		dirty = false;
	}

	return worldMatrix;
}

XMFLOAT4X4 Transform::GetWorldInverseTransposeMatrix()
{
	return XMFLOAT4X4();
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
