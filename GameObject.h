#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include "Mesh.h"
#include "Transform.h"
#include "ImgUtil.h"
#include <wrl/client.h>
#include "Window.h"
#include <memory>
#include "ImGui/imgui.h"

class GameObject
{
private:
	const char* name;
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Transform> transform;
	std::shared_ptr<GameObject> parentObj;
	std::vector<GameObject*> childObjs;
	DirectX::XMFLOAT4 tint;

public:
	GameObject(const char* name, std::shared_ptr<Mesh> mesh, std::shared_ptr<GameObject> parentObj);
	~GameObject();

	// -=| Getters and Setters |=-
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Transform> GetParentTransform();
	std::shared_ptr<Mesh> GetMesh();
	const char* GetName();
	DirectX::XMFLOAT4 GetTint();

	void SetObjAsChild(GameObject* GameObject);
	void SetTint(DirectX::XMFLOAT4 tintColor);

	void DrawImGui();
};

