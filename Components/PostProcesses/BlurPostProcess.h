#pragma once
#include "../../Interfaces/IPostProcessEffect.h"
#include "../../MainComponents/Window.h"
#include "../../ImGui/imgui.h"
#include "../../Helper/Settings.h"

#include "d3d11.h"
#include "DirectXMath.h"

class BlurPostProcess : public IPostProcessEffect
{
private:
	int* blurRadius;

public:
	BlurPostProcess(
		std::shared_ptr<SimpleVertexShader> ppVS,
		std::shared_ptr<SimplePixelShader> ppPS,
		Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler);
	void OnResize() override;
	void ClearRTV() override;
	void Draw() override;
	void SetRenderTarget() override;
	void DrawImGui() override;
};

