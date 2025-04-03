#pragma once
#include <memory>
#include "DirectXMath.h"
#include <wrl/client.h>
#include "d3d11.h"

#include "../MainComponents/Graphics.h"

#include "../Helper/LoadHelper.h"
#include "../Interfaces/IRenderable.h"

class Sky : public IRenderable
{
private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> cubeMapSRV;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthState;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;

	void CreateStates();

public:
	Sky(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, const char* path);
	Sky(Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState, ComPtr<ID3D11ShaderResourceView> cubeMap);
	Sky();

	std::shared_ptr<Mesh> GetMesh() override;
	std::shared_ptr<Material> GetMaterial() override;
	std::shared_ptr<Transform> GetTransform() override;

	void Draw() override;

};

