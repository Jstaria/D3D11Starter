#include "GameObject.h"

using namespace std;
using namespace DirectX;

GameObject::GameObject(const char* name, std::shared_ptr<Mesh> mesh, std::shared_ptr<Transform> parentTransform)
	: name(name), parentTransform(parentTransform), mesh(mesh)
{
	transform = make_shared<Transform>();
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
	return parentTransform;
}

std::shared_ptr<Mesh> GameObject::GetMesh()
{
	return mesh;
}

DirectX::XMFLOAT4 GameObject::GetTint()
{
	return tint;
}

void GameObject::SetTint(XMFLOAT4 tintColor)
{
	tint = tintColor;
}

void GameObject::DrawImGui()
{
	if (ImGui::TreeNode(name)) {
		{
			XMFLOAT3 pos = transform.get()->GetPosition();
			ImGui::DragFloat3(("Position##" + std::string(name)).c_str(), &pos.x, .001);
			transform.get()->SetPosition(pos);
		}

		{
			XMFLOAT3 rot = transform.get()->GetPitchYawRoll();
			ImGui::DragFloat3(("Rotation##" + std::string(name)).c_str(), &rot.x, .001);
			transform.get()->SetRotation(rot);
		}

		{
			XMFLOAT3 scale = transform.get()->GetScale();
			ImGui::DragFloat3(("Scale##" + std::string(name)).c_str(), &scale.x, .001);
			transform.get()->SetScale(scale);
		}

		{
			ImGui::ColorEdit4(("Tint##" + std::string(name)).c_str(), &tint.x, .001);
		}

		ImGui::TreePop();
	}
}
