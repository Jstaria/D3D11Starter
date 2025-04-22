#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "Graphics.h"

#include "Window.h"

#include "../ImGui/imgui.h"
#include "../Helper/SimpleShader.h"
#include "../Structures/ExternalData.h"
#include "../Components/Camera.h"
#include "../Components/Light.h"
#include "../Components/Sky.h"

namespace Renderer
{
	void Init();
	void AddObjectToRender(std::shared_ptr<IRenderable> gameObj);
	void SetCurrentCamera(std::shared_ptr<Camera> camera);
	void SetCurrentSky(std::shared_ptr<Sky> sky);
	void SetLights(std::vector<std::shared_ptr<Light>> lightList);
	void SetShadowVS(std::shared_ptr<SimpleVertexShader> shadowVS);
	void DrawRenderables();
	void DrawImGui();
	void UpdateRenderableList();

	// -=| Getters |=-
	std::shared_ptr<Camera> GetCamera();
};

