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
	SetPosition(position.x, position.y, position.z);
}

void Transform::SetRotation(float p, float y, float r, Angle angle)
{
	if (angle == Angle::DEGREES)
	{
		p *= Pi;
		y *= Pi;
		r *= Pi;
	}

	XMStoreFloat3(&rotation, XMVectorSet(p, y, r, 0.0f));
	NotifyOfCleanliness(true);
}

void Transform::SetRotation(DirectX::XMFLOAT3 rotation,Angle angle)
{
	SetRotation(rotation.x, rotation.y, rotation.z, angle);
}

void Transform::SetScale(float x, float y, float z)
{
	XMStoreFloat3(&scale, XMVectorSet(x, y, z, 0.0f));
	NotifyOfCleanliness(true);
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	SetScale(scale.x, scale.y, scale.z);
	
}
void Transform::SetScale(float scale)
{
	SetScale(scale, scale, scale);
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
DirectX::XMFLOAT3 Transform::GetWorldPosition() {
	
	XMFLOAT4X4 world = GetWorldMatrix();
	
	XMVECTOR pos = XMLoadFloat3(&position);
	XMMATRIX worldMat = XMLoadFloat4x4(&world);
	pos = XMVector3Transform(pos, worldMat);

	XMFLOAT3 positionVec;  XMStoreFloat3(&positionVec, pos);
	return positionVec;
}
XMFLOAT3 Transform::GetPitchYawRoll() { return rotation; }
XMFLOAT3 Transform::GetScale() { return scale; }

DirectX::XMFLOAT3 Transform::GetUp() { UpdateVectors(); return up; }
DirectX::XMFLOAT3 Transform::GetRight() { UpdateVectors(); return right; }
DirectX::XMFLOAT3 Transform::GetForward() { UpdateVectors(); return forward; }

XMFLOAT4X4 Transform::GetWorldMatrix()
{
	if (matricesDirty) {
		XMStoreFloat4(&quaternion, XMQuaternionRotationRollPitchYawFromVector(XMLoadFloat3(&rotation)));

		XMMATRIX trMatrix = XMMatrixTranslationFromVector(XMLoadFloat3(&position));
		XMMATRIX rotMatrix = XMMatrixRotationQuaternion(XMLoadFloat4(&quaternion));
		XMMATRIX scMatrix = XMMatrixScalingFromVector(XMLoadFloat3(&scale));
		XMMATRIX parentMatrix = XMMatrixIdentity();

		if (parentTransform != nullptr) {
			parentTransform->NotifyOfCleanliness(true);
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
	NotifyOfCleanliness(true);
}

void Transform::MoveAbsolute(DirectX::XMFLOAT3 offset)
{
	MoveAbsolute(offset.x, offset.y, offset.z);
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

void Transform::Rotate(float p, float y, float r, Angle angle)
{
	if (angle == Angle::DEGREES)
	{
		p *= Pi;
		y *= Pi;
		r *= Pi;
	}

	this->rotation.x += p;
	this->rotation.y += y;
	this->rotation.z += r;
	NotifyOfCleanliness(true);
}

void Transform::Rotate(DirectX::XMFLOAT3 rotation, Angle angle)
{
	Rotate(rotation.x, rotation.y, rotation.z, angle);
}

void Transform::Rotate(DirectX::XMFLOAT3 rotationAxis,float rotation, Angle angle, DirectX::XMFLOAT3 point)
{
	if (angle == Angle::DEGREES)
		rotation *= Pi;
	
	DirectX::XMMATRIX transformMat =
	DirectX::XMMatrixMultiply(
		DirectX::XMMatrixTranslation(-point.x, -point.y, -point.z), 
		XMMatrixMultiply(
			DirectX::XMMatrixRotationAxis(DirectX::XMLoadFloat3(&rotationAxis), rotation), 
			DirectX::XMMatrixTranslation(point.x, point.y, point.z)));

	XMVECTOR pos = XMLoadFloat3(&position);

	pos = XMVector3Transform(pos, transformMat);

	DirectX::XMStoreFloat3(&position, pos);
	NotifyOfCleanliness(true);
}

void Transform::LookAt(DirectX::XMFLOAT3 point)
{
	XMFLOAT3 position = GetPosition();
	XMVECTOR posVec = XMLoadFloat3(&position);
	XMVECTOR targetVec = XMLoadFloat3(&point);

	XMVECTOR toTarget = XMVector3Normalize(targetVec - posVec);

	float yaw = atan2f(XMVectorGetX(toTarget), XMVectorGetZ(toTarget));

	float pitch = asinf(-XMVectorGetY(toTarget));

	SetRotation(pitch, yaw, 0.0f, Angle::RADIANS);
}

void Transform::LookAtRelative(DirectX::XMFLOAT3 direction)
{
	XMFLOAT3 lookat = position;
	lookat.x -= direction.x;
	lookat.y -= direction.y;
	lookat.z -= direction.z;

	LookAt(lookat);
}

void Transform::Scale(float x, float y, float z)
{
	XMStoreFloat3(&scale, XMVectorSet(scale.x * x, scale.y * y, scale.z * z, 0.0f));
	NotifyOfCleanliness(true);
}

void Transform::Scale(DirectX::XMFLOAT3 scale)
{
	Scale(scale.x, scale.y, scale.z);
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

	XMStoreFloat3(&up, XMVector3Rotate(XMVectorSet(0, 1, 0, 0), rotQuat));
	XMStoreFloat3(&right, XMVector3Rotate(XMVectorSet(1, 0, 0, 0), rotQuat));
	XMStoreFloat3(&forward, XMVector3Rotate(XMVectorSet(0, 0, 1, 0), rotQuat));
}
