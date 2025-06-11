#include "BlurPostProcess.h"

BlurPostProcess::BlurPostProcess(
	std::shared_ptr<SimpleVertexShader> ppVS,
	std::shared_ptr<SimplePixelShader> ppPS,
	Microsoft::WRL::ComPtr<ID3D11SamplerState> ppSampler)
	: IPostProcessEffect(ppVS, ppPS, ppSampler), blurRadius(0)
{
}

void BlurPostProcess::OnResize()
{
	// Describe the texture we're creating
	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.Width = (int)Window::Width();
	textureDesc.Height = (int)Window::Height();
	textureDesc.ArraySize = 1;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.MipLevels = 1;
	textureDesc.MiscFlags = 0;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	// Create the resource (no need to track it after the views are created below)
	Microsoft::WRL::ComPtr<ID3D11Texture2D> ppTexture;
	Graphics::Device->CreateTexture2D(&textureDesc, 0, ppTexture.GetAddressOf());

	// Create the Render Target View
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D.MipSlice = 0;
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	Graphics::Device->CreateRenderTargetView(
		ppTexture.Get(),
		&rtvDesc,
		ppRTV.ReleaseAndGetAddressOf());
	// Create the Shader Resource View
	// By passing it a null description for the SRV, we
	// get a "default" SRV that has access to the entire resource
	Graphics::Device->CreateShaderResourceView(
		ppTexture.Get(),
		0,
		ppSRV.ReleaseAndGetAddressOf());

	blurRadius = &Settings::BlurStrength;
}

void BlurPostProcess::ClearRTV()
{
	float color[4] = { 0.0f, 0.0f, 0.0f, 0.0f };

	Graphics::Context->ClearRenderTargetView(ppRTV.Get(), color);
}

void BlurPostProcess::Draw()
{
	// Activate shaders and bind resources
	// Also set any required cbuffer data (not shown)
	ppVS->SetShader();
	ppPS->SetShader();

	ppPS->SetInt("blurRadius", *blurRadius);
	ppPS->SetFloat("pixelWidth", 1.0f / Window::Width());
	ppPS->SetFloat("pixelHeight", 1.0f / Window::Height());

	ppPS->SetShaderResourceView("Pixels", ppSRV.Get());
	ppPS->SetSamplerState("ClampSampler", ppSampler.Get());

	ppPS->CopyAllBufferData();

	Graphics::Context->Draw(3, 0); // Draw exactly 3 vertices (one triangle)
}

void BlurPostProcess::SetRenderTarget()
{
	Graphics::Context->OMSetRenderTargets(1, ppRTV.GetAddressOf(), Graphics::DepthBufferDSV.Get());
}

void BlurPostProcess::DrawImGui()
{
	if (ImGui::TreeNode("Blur")) {
		
		ImGui::SliderInt("Blur Radius", blurRadius , 0, 30);

		ImGui::TreePop();
	}
}
