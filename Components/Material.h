#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl/client.h>

#include "../Helper/SimpleShader.h"
#include "../Helper/GlobalVar.h"
#include "../MainComponents/Window.h"

class Material
{
private:
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	DirectX::XMFLOAT4 color;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture;

public:
	Material(std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, DirectX::XMFLOAT4 color);

	// -=| Getters |=-
	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();
	DirectX::XMFLOAT4 GetColor();
};

