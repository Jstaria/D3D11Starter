#include "Light.h"

Light::Light(const char* name, bool canUpdate, bool isActive, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, float intensity, float range)
	: GameObject(name, nullptr, nullptr, nullptr), isActive(isActive), canUpdate(canUpdate)
{
	lightStruct = {};
	lightStruct.Type = LIGHT_TYPE_POINT;
	lightStruct.Color = color;
	lightStruct.Position = position;
	lightStruct.Intensity = intensity;
	lightStruct.Range = range;

	transform->SetPosition(position);
	transform->SetScale(0.1f);
}

Light::Light(const char* name, bool canUpdate, bool isActive, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction, float intensity)
	: GameObject(name, nullptr, nullptr, nullptr), isActive(isActive), canUpdate(canUpdate)
{
	lightStruct = {};
	lightStruct.Type = LIGHT_TYPE_DIRECTIONAL;
	lightStruct.Color = color;
	lightStruct.Intensity = intensity;
	lightStruct.Direction = direction;
}

Light::Light(const char* name, bool canUpdate, bool isActive, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction, float range, float intensity, float innerAngle, float outerAngle)
	: GameObject(name, nullptr, nullptr, nullptr), isActive(isActive), canUpdate(canUpdate)
{
	lightStruct = {};
	lightStruct.Type = LIGHT_TYPE_SPOT;
	lightStruct.Position = position;
	lightStruct.Color = color;
	lightStruct.Intensity = intensity;
	lightStruct.Range = range;
	lightStruct.Direction = direction;
	lightStruct.SpotInnerAngle = innerAngle;
	lightStruct.SpotOuterAngle = outerAngle;

	transform->SetScale(0.1f);
	transform->SetPosition(position);
}

void Light::SetLightColor(DirectX::XMFLOAT3 color)
{
	material->SetTint(DirectX::XMFLOAT4(color.x, color.y, color.z, 1));
	lightStruct.Color = color;
}

void Light::SetRange(float range) { lightStruct.Range = range; }

void Light::UpdateLightColor() { SetLightColor(lightStruct.Color); }

void Light::SetActive(bool active) { isActive = active; }
bool Light::GetActive() { return isActive; }

void Light::Draw()
{
	if (!isActive || lightStruct.Type == LIGHT_TYPE_DIRECTIONAL || !Debug::ShowLightsMesh) return;
	
	drawable->Draw();
}

void Light::DrawImGui()
{
	if (ImGui::TreeNode(name)) {

		if (parentObj != nullptr)
			ImGui::Text("Parent GameObject: %s", parentObj->GetName());

		const std::string type = lightStruct.Type == 0 ? "Directional" : lightStruct.Type == 1 ? "Point" : "Spot";

		ImGui::Text(("Light Type: " + type).c_str());

		ImGui::Checkbox("Is Active", &isActive);

		if (lightStruct.Type != LIGHT_TYPE_DIRECTIONAL) {
			DirectX::XMFLOAT3 pos = transform->GetPosition();
			ImGui::DragFloat3(("Position##" + std::string(name)).c_str(), &pos.x, .01f);
			lightStruct.Position = !canUpdate ? pos : lightStruct.Position;
			transform->SetPosition(pos);

			ImGui::DragFloat(("Light Range##" + std::string(name)).c_str(), &lightStruct.Range, 0.01f, 100.0f);
		}

		if (lightStruct.Type != LIGHT_TYPE_POINT) {
			DirectX::XMFLOAT3 dir = lightStruct.Direction;
			ImGui::DragFloat3(("Direction##" + std::string(name)).c_str(), &dir.x, .01f);
			lightStruct.Direction = dir;
		}

		if (lightStruct.Type == LIGHT_TYPE_SPOT) {
			ImGui::DragFloat(("Inner Angle##" + std::string(name)).c_str(), &lightStruct.SpotInnerAngle, 0.1f, 0.01f, lightStruct.SpotOuterAngle - 0.01f);
			ImGui::DragFloat(("Outer Angle##" + std::string(name)).c_str(), &lightStruct.SpotOuterAngle, 0.1f, lightStruct.SpotInnerAngle + 0.01f, 3.14159f);
		}

		ImGui::DragFloat(("Light Intensity##" + std::string(name)).c_str(), &lightStruct.Intensity, 0.1f, 0.1f, 5.0f);
		ImGui::ColorEdit3(("Tint##" + std::string(name)).c_str(), &lightStruct.Color.x);
		material->SetTint(DirectX::XMFLOAT4(lightStruct.Color.x, lightStruct.Color.y, lightStruct.Color.z, 1));

		ImGui::TreePop();
	}
}

void Light::FixedUpdate(float deltaTime)
{
	if (!canUpdate || !isActive) return;

	transform->SetDirty(true);

	lightStruct.Position = transform->GetWorldPosition();
}

LightStruct Light::GetStruct() { return lightStruct; }
