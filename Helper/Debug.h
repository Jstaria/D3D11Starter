#pragma once

#include <DirectXMath.h>
#include <d3d11.h>
#include <wrl/client.h>
#include "../MainComponents/Graphics.h"
#include "../MainComponents/Window.h"
#include "../ImGui/imgui.h"

#include "../Structures/Vertex.h"

using namespace Microsoft::WRL;

namespace Debug {
	
	// --= Global Var =--
	
	inline bool ShowWireFrame;
	inline bool ShowMesh;
	inline bool ShowLightsMesh;
	inline int ShadowMapResolution;
	inline float ShadowMapSize;
	inline bool ShowNavMesh;

	inline ComPtr<ID3D11RasterizerState> RasterizerFillState;
	inline ComPtr<ID3D11RasterizerState> RasterizerWFState;

	// --= Initialize =--

	void Initialize();
	void CreateRasterizerStates();
	void DrawImGui();
}

