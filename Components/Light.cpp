#include "Light.h"

Light::Light(const char* name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, float intensity, float range)
	: GameObject(name, nullptr, nullptr, nullptr), isActive(true)
{
	lightStruct = {};
	lightStruct.Type = LIGHT_TYPE_POINT;
	lightStruct.Color = color;
	lightStruct.Position = position;
	lightStruct.Intensity = intensity;
	lightStruct.Range = range;
}

Light::Light(const char* name, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction, float intensity)
	: GameObject(name, nullptr, nullptr, nullptr), isActive(true)
{
	lightStruct = {};
	lightStruct.Type = LIGHT_TYPE_DIRECTIONAL;
	lightStruct.Color = color;
	lightStruct.Intensity = intensity;
	lightStruct.Direction = direction;
}

Light::Light(const char* name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction, DirectX::XMFLOAT2 padding, float range, float intensity, float innerAngle, float outerAngle)
	: GameObject(name, nullptr, nullptr, nullptr), isActive(true)
{
	lightStruct = {};
	lightStruct.Type = LIGHT_TYPE_SPOT;
	lightStruct.Position = position;
	lightStruct.Color = color;
	lightStruct.Intensity = intensity;
	lightStruct.Range = range;
	lightStruct.Direction = direction;
	lightStruct.Padding = padding;
	lightStruct.SpotInnerAngle = innerAngle;
	lightStruct.SpotOuterAngle = outerAngle;
}

void Light::SetActive(bool active) { isActive = active; }
bool Light::GetActive() { return isActive; }

LightStruct Light::GetStruct() { return lightStruct; }
