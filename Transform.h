#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include "Mesh.h"
#include "ImgUtil.h"
#include <wrl/client.h>
#include "Window.h"

class Transform
{
private:
	// -=| Transform Data |=-
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 scale;
	DirectX::XMFLOAT4 quaternion;
	DirectX::XMFLOAT3 rotation; //	float pitch; float yaw; float roll;

	// -=| Matrix |=-
	DirectX::XMFLOAT4X4 worldMatrix;
	DirectX::XMFLOAT4X4 worldInverseTransposeMatrix;
	bool dirty;

public:
	Transform();

	void SetPosition(float x, float y, float z);
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetRotation(float p, float y, float r);
	void SetRotation(DirectX::XMFLOAT3 rotation); // XMFLOAT4 for quaternion
	void SetScale(float x, float y, float z);
	void SetScale(DirectX::XMFLOAT3 scale);
	void SetDirty(bool value);

	DirectX::XMFLOAT3 GetPosition();
	DirectX::XMFLOAT3 GetPitchYawRoll(); // XMFLOAT4 GetRotation() for quaternion
	DirectX::XMFLOAT3 GetScale();
	DirectX::XMFLOAT4X4 GetWorldMatrix();
	DirectX::XMFLOAT4X4 GetWorldInverseTransposeMatrix();
	bool GetDirty();

	void MoveAbsolute(float x, float y, float z); // Based on world axis
	void MoveAbsolute(DirectX::XMFLOAT3 offset);
	void MoveRelative(float x, float y, float z); // Based on my rotation
	void MoveRelative(DirectX::XMFLOAT3 offset);
	void Rotate(float p, float y, float r);
	void Rotate(DirectX::XMFLOAT3 rotation);
	void Scale(float x, float y, float z);
	void Scale(DirectX::XMFLOAT3 scale);
};
