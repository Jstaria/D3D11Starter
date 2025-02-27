#pragma once
#include <vector>
#include <memory>
#include <functional>
#include "Graphics.h"

#include "../Structures/ExternalData.h"
#include "../Components/Camera.h"

namespace Renderer
{
	void Init();
	void AddObjectToRender(std::shared_ptr<IRenderable> gameObj);
	void SetCurrentCamera(std::shared_ptr<Camera> camera);
	void DrawRenderables();
	void UpdateRenderableList();

	// -=| Getters |=-
	std::shared_ptr<Camera> GetCamera();
};

