#include "Debug.h"

void Debug::Initialize()
{
	CreateRasterizerStates();
}

void Debug::CreateRasterizerStates()
{
	// --- Create Rasterizer States --- 
	{
		D3D11_RASTERIZER_DESC rasterizer = {};
		rasterizer.FillMode = D3D11_FILL_SOLID;
		rasterizer.CullMode = D3D11_CULL_BACK;
		rasterizer.DepthClipEnable = true;

		Graphics::Device->CreateRasterizerState(&rasterizer, RasterizerFillState.GetAddressOf());

		D3D11_RASTERIZER_DESC wfRasterizer = rasterizer;
		wfRasterizer.FillMode = D3D11_FILL_WIREFRAME;

		Graphics::Device->CreateRasterizerState(&wfRasterizer, RasterizerWFState.GetAddressOf());
	}
}
