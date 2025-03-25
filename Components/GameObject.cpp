#include "GameObject.h"

using namespace std;
using namespace DirectX;

GameObject::GameObject(const char* name, std::shared_ptr<Mesh> mesh, std::shared_ptr<GameObject> parentObj, std::shared_ptr<Material> material)
	: name(name), parentObj(parentObj)
{
	this->rendererType = IRenderable::RenderType::Object;
	this->mesh = mesh;
	this->material = material;

	this->transform = make_shared<Transform>();

	this->transform.get()->SetParentTransform(parentObj != nullptr ? parentObj.get()->GetTransform() : nullptr);

	if (parentObj != nullptr) {
		parentObj.get()->SetObjAsChild(this);
	}

	tint = XMFLOAT4(1, 1, 1, 1);
}

GameObject::~GameObject()
{
}

std::shared_ptr<Transform> GameObject::GetTransform() { return transform; }
std::shared_ptr<Transform> GameObject::GetParentTransform() { return transform.get()->GetParentTransform(); }
std::shared_ptr<Mesh> GameObject::GetMesh() { return mesh; }
std::shared_ptr<Material> GameObject::GetMaterial() { return material; }
const char* GameObject::GetName() { return name; }
DirectX::XMFLOAT4 GameObject::GetTint() { return tint; }

void GameObject::SetObjAsChild(GameObject* child) { childObjs.push_back(child); }
void GameObject::SetTint(XMFLOAT4 tintColor) { tint = tintColor; }
void GameObject::SetMaterial(std::shared_ptr<Material> material) { this->material = material; }

void GameObject::DrawImGui(std::map<const char*, std::shared_ptr<Material>> materials, std::vector<const char*> materialKeys)
{
	if (ImGui::TreeNode(name)) {

		if (parentObj != nullptr)
			ImGui::Text("Parent GameObject: %s", parentObj->GetName());

		ImGui::Text("Mesh: %s", mesh->GetName());
		ImGui::Text("Material : %s", material->GetName());

		int index = material->GetMatIndex();

		ImGui::Text("Material Select:");
		ImGui::SameLine();

		if (ImGui::Combo("##MapCombo", &index, materialKeys.data(), materialKeys.size())) {
			material = materials[materialKeys[index]];
		}

		material->ImGuiDraw();

		{
			XMFLOAT3 pos = transform->GetPosition();
			ImGui::DragFloat3(("Position##" + std::string(name)).c_str(), &pos.x, .01f);
			transform->SetPosition(pos);
		}

		{
			XMFLOAT3 rot = transform->GetPitchYawRoll();
			ImGui::DragFloat3(("Rotation##" + std::string(name)).c_str(), &rot.x, .01f);
			transform->SetRotation(rot, Angle::PI);
		}

		{
			XMFLOAT3 scale = transform->GetScale();
			ImGui::DragFloat3(("Scale##" + std::string(name)).c_str(), &scale.x, .01f);
			transform->SetScale(scale);
		}

		{
			ImGui::ColorEdit4(("Tint##" + std::string(name)).c_str(), &tint.x, .01f);
		}

		ImGui::TreePop();
	}
}

IRenderable::RenderType GameObject::GetRenderType() { return rendererType; }

void GameObject::Draw()
{
	mesh->Draw();
}

//void GameObject::AddComponent(std::shared_ptr<Component>)
//{
//}
