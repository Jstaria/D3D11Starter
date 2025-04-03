#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "Graphics.h"

#include "../Structures/ExternalData.h"
#include "../Components/Camera.h"
#include "../Components/Light.h"
#include "../Components/Sky.h"

namespace Renderer
{
	void Init();
	void AddObjectToRender(std::shared_ptr<IRenderable> gameObj);
	void SetCurrentCamera(std::shared_ptr<Camera> camera);
	void SetLights(std::vector<std::shared_ptr<Light>> lightList);
	void DrawRenderables();
	void UpdateRenderableList();

	// -=| Getters |=-
	std::shared_ptr<Camera> GetCamera();
};

