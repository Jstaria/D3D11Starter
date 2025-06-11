#pragma once
#include <memory>
#include "DirectXMath.h"
#include <wrl/client.h>
#include "d3d11.h"

#include "../MainComponents/Graphics.h"
#include "../Components/Mesh.h"
#include "../Structures/ExternalData.h"
#include "../Helper/SimpleShader.h"

#include "../Helper/LoadHelper.h"

class Sky
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	std::shared_ptr<IDrawable> mesh;
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;

	void CreateStates();

public:
	Sky(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, const wchar_t* path, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<IDrawable> mesh);
	Sky(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, ComPtr<ID3D11ShaderResourceView> cubeMap, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<IDrawable> mesh);
	Sky();

	void Draw(ExternalData data);

};

