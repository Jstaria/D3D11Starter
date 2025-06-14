#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <memory>
#include <vector>
#include <functional>
#include "ImgUtil.h"
#include <wrl/client.h>

#include "Mesh.h"

#include "../MainComponents/Window.h"

#define Pi 0.01745329251f;

enum Angle {
	RADIANS,
	DEGREES
};

class Transform
{
private:
	// -=| Transform Data |=-
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT3 rotation; //	float pitch; float yaw; float roll;
	DirectX::XMFLOAT3 up;
	DirectX::XMFLOAT3 right;
	DirectX::XMFLOAT3 forward;

	// -=| Parent Transform |=-
	std::shared_ptr<Transform> parentTransform;
	std::vector<Transform*> childTransforms;
	void NotifyOfCleanliness(bool success);
	void OnClean(bool success);
	void UpdateVectors();

	// -=| Matrix |=-
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
	bool matricesDirty;
	bool vectorsDirty; // for up,right,forward

public:
	Transform();
	~Transform();

	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(float p, float y, float r, Angle angle);
	void SetRotation(DirectX::XMFLOAT3 rotation, Angle angle); // XMFLOAT4 for quaternion
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);
	void SetScale(float scale);
	void SetParentTransform(std::shared_ptr<Transform> transform);
	void SetChildTransform(Transform* transform);
	void SetDirty(bool value);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetWorldPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll(); // XMFLOAT4 GetRotation() for quaternion
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	std::shared_ptr<Transform> GetParentTransform();
	bool GetDirty();
	DirectX::XMFLOAT3 GetUp();
	DirectX::XMFLOAT3 GetRight();
	DirectX::XMFLOAT3 GetForward();

	void MoveAbsolute(float x, float y, float z); // Based on world axis
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z); // Based on my rotation
	void MoveRelative(DirectX::XMFLOAT3 offset);
	//void LookAt(DirectX::XMFLOAT3 position);
	void Rotate(float p, float y, float r, Angle angle);
	void Rotate(DirectX::XMFLOAT3 rotation, Angle angle);
	void Rotate(DirectX::XMFLOAT3 rotationAxis, float rotation, Angle angle, DirectX::XMFLOAT3 point);
	void LookAt(DirectX::XMFLOAT3 point);
	void LookAtRelative(DirectX::XMFLOAT3 direction);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);
};

