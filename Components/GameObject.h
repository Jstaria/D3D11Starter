#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include "ImgUtil.h"
#include <wrl/client.h>
#include <memory>

#include "Mesh.h"
#include "Transform.h"
#include "Material.h"

#include "../Interfaces/IRenderable.h"
#include "../MainComponents/Window.h"
#include "../ImGui/imgui.h"

class GameObject : public IRenderable
{
private:
	const char* name;
	std::shared_ptr<GameObject> parentObj;
	std::vector<GameObject*> childObjs;
	DirectX::XMFLOAT4 tint;

public:
	GameObject(const char* name, std::shared_ptr<Mesh> mesh, std::shared_ptr<GameObject> parentObj, std::shared_ptr<Material> material);
	~GameObject();

	// -=| Getters and Setters |=-
	std::shared_ptr<Transform> GetTransform();
	std::shared_ptr<Transform> GetParentTransform();
	std::shared_ptr<Mesh> GetMesh();
	std::shared_ptr<Material> GetMaterial();
	const char* GetName();
	DirectX::XMFLOAT4 GetTint();

	void SetObjAsChild(GameObject* GameObject);
	void SetTint(DirectX::XMFLOAT4 tintColor);

	void DrawImGui();

	// Inherited via IRenderable
	IRenderable::RenderType GetRenderType() override;

	// Inherited via IRenderable
	void Draw() override;
};

