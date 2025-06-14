#include "Debug.h"

void Debug::Initialize()
{
	CreateRasterizerStates();

	ShowLightsMesh = false;

	ShadowMapResolution = 4096;
	ShadowMapSize = 160.0f;
	ShowNavMesh = true;
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

void Debug::DrawImGui()
{
	if (ImGui::CollapsingHeader("Debug Information")) {
		ImGui::SetWindowFontScale(0.9f);
		ImGui::Text("(Note: Meshes override this debug toggle)");
		ImGui::SetWindowFontScale(1.0f);
		ImGui::Checkbox("Show Wireframes", &ShowWireFrame);
		ImGui::Checkbox("Show Meshes", &ShowMesh);
		ImGui::Checkbox("Show Light Meshes", &ShowLightsMesh);
		ImGui::Checkbox("Show Nav Meshes", &ShowNavMesh);
		if (ImGui::Button("Change Window Size: 720x720")) {
			Window::AdjustWindowSize(720, 720);
		}
	}
}
