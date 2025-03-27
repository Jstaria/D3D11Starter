#pragma once

#include "../Structures/LightStruct.h"
#include "GameObject.h"

#include "DirectXMath.h"

class Light : public GameObject {
protected:
	LightStruct lightStruct;
	bool isActive;

public:
	Light(const char* name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, float intensity, float range);
	Light(const char* name, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction, float intensity);
	Light(const char* name, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction, float range, float intensity, float innerAngle, float outerAngle);

	void SetActive(bool active);
	bool GetActive();

	void Draw() override;
	void DrawImGui();

	LightStruct GetStruct();
};