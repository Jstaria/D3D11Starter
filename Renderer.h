#pragma once
#include <vector>
#include "GameObject.h"
#include <memory>
#include <functional>
#include "ExternalData.h"
#include "Graphics.h"
#include "Camera.h"


namespace Renderer
{
	void Init();
	void AddObjectToRender(std::shared_ptr<GameObject> gameObj);
	void SetCurrentCamera(std::shared_ptr<Camera> camera);
	void DrawObjects();
	void UpdateObjectList();

	// -=| Getters |=-
	std::shared_ptr<Camera> GetCamera();
};

