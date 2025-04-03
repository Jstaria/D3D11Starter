#include "Sky.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

void Sky::CreateStates()
{
	{
		D3D11_RASTERIZER_DESC desc{};

		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.DepthClipEnable = true;
		Graphics::Device->CreateRasterizerState(&desc, rasterizerState.GetAddressOf());
	}

	{
		D3D11_DEPTH_STENCIL_DESC desc{};

		desc.DepthEnable = true;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		desc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;

		Graphics::Device->CreateDepthStencilState(&desc, depthState.GetAddressOf());
	}
}

Sky::Sky(ComPtr<ID3D11SamplerState> samplerState, const char* path)
	: sampler(samplerState)
{
	cubeMapSRV = LoadHelper::CreateCubemap(path);
	
	CreateStates();
}

Sky::Sky(ComPtr<ID3D11SamplerState> samplerState, ComPtr<ID3D11ShaderResourceView> cubeMap)
	: sampler(samplerState), cubeMapSRV(cubeMap)
{
	CreateStates();
}

Sky::Sky()
{
}

void Sky::SetShadersAndMesh(std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, std::shared_ptr<Mesh> mesh)
{
	this->vs = vs;
	this->ps = ps;
	this->mesh = mesh;
}

void Sky::Draw(ExternalData data)
{
	Graphics::Context->RSSetState(rasterizerState.Get());
	Graphics::Context->OMSetDepthStencilState(depthState.Get(), 0);

	vs->SetShader();
	ps->SetShader();

	vs->SetMatrix4x4("view", data.viewMatrix);
	vs->SetMatrix4x4("projection", data.projMatrix);

	ps->SetShaderResourceView("CubeMap", cubeMapSRV);
	ps->SetSamplerState("BasicSampler", sampler);

	vs->CopyAllBufferData();
	ps->CopyAllBufferData();

	mesh->Draw();

	Graphics::Context->RSSetState(0);
	Graphics::Context->OMSetDepthStencilState(0, 0);
}

