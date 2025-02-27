#pragma once
#include <memory>
#include "../Helper/SimpleShader.h"

class Material
{
private:
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	DirectX::XMFLOAT4 color;

public:
	Material(std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, DirectX::XMFLOAT4 color);

	// -=| Getters |=-
	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();
	DirectX::XMFLOAT4 GetColor();
};

