#include "GameObject.h"

using namespace std;
using namespace DirectX;

GameObject::GameObject(const char* name, std::shared_ptr<Mesh> mesh, std::shared_ptr<GameObject> parentObj)
	: name(name), mesh(mesh), parentObj(parentObj)
{
	transform = make_shared<Transform>();
	
	transform.get()->SetParentTransform(parentObj != nullptr ? parentObj.get()->GetTransform() : nullptr);

	if (parentObj != nullptr) {
		parentObj.get()->SetObjAsChild(this);
	}

	tint = XMFLOAT4(1, 1, 1, 1);
}

GameObject::~GameObject()
{
}

std::shared_ptr<Transform> GameObject::GetTransform()
{
	return transform;
}

std::shared_ptr<Transform> GameObject::GetParentTransform()
{
	return transform.get()->GetParentTransform();
}

std::shared_ptr<Mesh> GameObject::GetMesh()
{
	return mesh;
}

const char* GameObject::GetName()
{
	return name;
}

DirectX::XMFLOAT4 GameObject::GetTint()
{
	return tint;
}

void GameObject::SetObjAsChild(GameObject* child)
{
	childObjs.push_back(child);
}

void GameObject::SetTint(XMFLOAT4 tintColor)
{
	tint = tintColor;
}

void GameObject::DrawImGui()
{
	if (ImGui::TreeNode(name)) {

		if (parentObj != nullptr)
			ImGui::Text("Parent GameObject: %s", parentObj.get()->GetName());

		ImGui::Text("Mesh: %s", mesh.get()->GetName());

		{
			XMFLOAT3 pos = transform.get()->GetPosition();
			ImGui::DragFloat3(("Position##" + std::string(name)).c_str(), &pos.x, .01f);
			transform.get()->SetPosition(pos);
		}

		{
			XMFLOAT3 rot = transform.get()->GetPitchYawRoll();
			ImGui::DragFloat3(("Rotation##" + std::string(name)).c_str(), &rot.x, .01f);
			transform.get()->SetRotation(rot);
		}

		{
			XMFLOAT3 scale = transform.get()->GetScale();
			ImGui::DragFloat3(("Scale##" + std::string(name)).c_str(), &scale.x, .01f);
			transform.get()->SetScale(scale);
		}

		{
			ImGui::ColorEdit4(("Tint##" + std::string(name)).c_str(), &tint.x, .01f);
		}

		ImGui::TreePop();
	}
}
