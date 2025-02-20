#pragma once

#include "Transform.h"
#include <DirectXMath.h>
#include <memory>
#include "Input.h"
#include "ImGui/imgui.h"
#include "GameObject.h"

class Camera
{
protected:
	DirectX::XMFLOAT4X4 viewMatrix;
	DirectX::XMFLOAT4X4 projMatrix;

	std::shared_ptr<Transform> transform;
	std::shared_ptr<GameObject> parentObj;
	const char* name;
	// Camera Var
	float FOV;

	float nearClipPlane;
	float farClipPlane;
	float aspectRatio;

public:
	Camera(
		const char* name,
		DirectX::XMFLOAT3 pos, 
		float FOV, 
		float aspectRatio, 
		float nearClipPlane, 
		float farClipPlane);
	~Camera();

	// -=| Updates |=-

	virtual void Update(float dt);
	void UpdateViewMatrix();
	void UpdateProjectionMatrix(float aspectRatio);

	// -=| Getters |=-
	DirectX::XMFLOAT4X4 GetView();
	DirectX::XMFLOAT4X4 GetProjection();
	std::shared_ptr<Transform> GetTransform();
	const char* GetName();

	// -=| ImGui |=-
	virtual void UIDraw();
};

