#include "PostProcessManager.h"

namespace {
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler;
	std::shared_ptr<SimpleVertexShader> ppVS;

	std::vector<std::shared_ptr<IPostProcessEffect>> ppEffects;
}

void PostProcessManager::SetInitialTarget()
{
	if (ppEffects.size() < 1) return;

	for (auto& effect : ppEffects)
		//effect->ClearRTV();

	ppEffects[0]->SetRenderTarget();
}

void PostProcessManager::ResetRenderTarget()
{
	Graphics::Context->OMSetRenderTargets(1, Graphics::BackBufferRTV.GetAddressOf(), 0);
}

void PostProcessManager::Init()
{
	D3D11_SAMPLER_DESC ppSampDesc = {};
	ppSampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	ppSampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	ppSampDesc.MaxLOD = D3D11_FLOAT32_MAX;
	Graphics::Device->CreateSamplerState(&ppSampDesc, ppSampler.GetAddressOf());

	// Post process shaders
	std::shared_ptr<SimplePixelShader> PPBlurPS = std::make_shared<SimplePixelShader>(Graphics::Device, Graphics::Context, FixPath(L"PPBlurPS.cso").c_str());
	std::shared_ptr<SimpleVertexShader> PostProcessVS = std::make_shared<SimpleVertexShader>(Graphics::Device, Graphics::Context, FixPath(L"PostProcessVS.cso").c_str());

	AddPostProcess(std::make_shared<BlurPostProcess>(PostProcessVS, PPBlurPS, ppSampler));

	for (auto& effect : ppEffects)
		effect->OnResize();
}

void PostProcessManager::OnResize()
{
	for (auto& effect : ppEffects)
		effect->OnResize();
}

void PostProcessManager::DrawFinal()
{
	if (ppEffects.size() < 1) return;

	//for (int i = 0; i < ppEffects.size() - 1; i++) {
	//	
	//	std::shared_ptr<IPostProcessEffect> effect = ppEffects[i];
	//	std::shared_ptr<IPostProcessEffect> nextEffect = ppEffects[i + 1];

	//	nextEffect->SetRenderTarget();
	//	effect->Draw();
	//}

	ResetRenderTarget();

	//ppEffects[ppEffects.size() - 1]->Draw();
	ppEffects[0]->Draw();
}

void PostProcessManager::DrawImGui()
{
	if (ImGui::TreeNode("Post Processes")) {
		for (auto& effect : ppEffects)
			effect->DrawImGui();

		ImGui::TreePop();
	}
}

void PostProcessManager::AddPostProcess(std::shared_ptr<IPostProcessEffect> ppEffect)
{
	ppEffects.push_back(ppEffect);
}
