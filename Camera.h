#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <memory>
#include "Input.h"

class Camera
{
private:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	std::shared_ptr<Transform> transform;

	// Camera Var
	float FOV;
	float movementSpeed;
	float mouseLookSpeed;
	float nearClipPlane;
	float farClipPlane;
	float aspectRatio;

public:
	Camera(
		DirectX::XMFLOAT3 pos, 
		float moveSpeed, 
		float lookSpeed, 
		float FOV, 
		float aspectRatio, 
		float nearClipPlane, 
		float farClipPlane);
	~Camera();

	// -=| Updates |=-

	void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	// -=| Getters |=-
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	std::shared_ptr<Transform> GetTransform();
};

