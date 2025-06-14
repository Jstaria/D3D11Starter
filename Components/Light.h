#pragma once

#include "../Structures/LightStruct.h"
#include "GameObject.h"

#include "DirectXMath.h"

class Light : public GameObject {
protected:
	LightStruct lightStruct;
	bool isActive;
	bool canUpdate;

public:
	Light(const char* name, bool canUpdate, bool isActive, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, float intensity, float range);
	Light(const char* name, bool canUpdate, bool isActive, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction, float intensity);
	Light(const char* name, bool canUpdate, bool isActive, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 color, DirectX::XMFLOAT3 direction, float range, float intensity, float innerAngle, float outerAngle);

	void SetLightColor(DirectX::XMFLOAT3 color);
	void SetRange(float range);
	void UpdateLightColor();
	void SetActive(bool active);
	bool GetActive();

	void Draw() override;
	void DrawImGui();
	void FixedUpdate(float deltaTime) override;

	LightStruct GetStruct();
};