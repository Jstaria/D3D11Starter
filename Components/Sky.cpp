#include "Sky.h"

using namespace std;
using namespace DirectX;
using namespace Microsoft::WRL;

void Sky::CreateStates()
{
	transform = make_shared<Transform>();

	{
		D3D11_RASTERIZER_DESC desc{};

		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		Graphics::Device->CreateRasterizerState(&desc, &rasterizerState);
	}

	{
		D3D11_DEPTH_STENCIL_DESC desc{};

		desc.DepthEnable = true;
		desc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;
		Graphics::Device->CreateDepthStencilState(&desc, &depthState);
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

std::shared_ptr<Mesh> Sky::GetMesh() { return mesh; }
std::shared_ptr<Material> Sky::GetMaterial() { return material; }
std::shared_ptr<Transform> Sky::GetTransform() { return transform; }

void Sky::Draw()
{
	Graphics::Context->RSSetState(rasterizerState.Get());
	Graphics::Context->OMSetDepthStencilState(depthState.Get(), 0);

	mesh->Draw();

	Graphics::Context->RSSetState(nullptr);
	Graphics::Context->OMSetDepthStencilState(nullptr, 0);
}

