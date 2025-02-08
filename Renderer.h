#pragma once
#include <vector>
#include "GameObject.h"
#include <memory>
#include <functional>
#include "ExternalData.h"
#include "Graphics.h"

namespace Renderer
{
	void Init();
	void AddObjectToRender(std::shared_ptr<GameObject> gameObj);
	void DrawObjects();
	void UpdateObjectList();
};

