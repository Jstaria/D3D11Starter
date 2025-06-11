#pragma once
#include <memory>

#include "../Helper/SimpleShader.h"
#include "../MainComponents/Graphics.h"

class IPostProcessEffect {
public:
	IPostProcessEffect(
		std::shared_ptr<SimpleVertexShader> ppVS,
		std::shared_ptr<SimplePixelShader> ppPS,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler)
		: ppVS(ppVS), ppPS(ppPS), ppSampler(ppSampler) {}

	virtual void OnResize() = 0;
	virtual void SetRenderTarget() = 0;
	virtual void ClearRTV() = 0;
	virtual void Draw() = 0;
	virtual void DrawImGui() = 0;

protected:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;

	std::shared_ptr<SimplePixelShader> ppPS;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> ppRTV; 
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> ppSRV;
};