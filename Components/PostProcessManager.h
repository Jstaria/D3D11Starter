#pragma once

#include <memory>
#include <vector>

#include "../Interfaces/IPostProcessEffect.h"
#include "../Helper/SimpleShader.h"
#include "../Helper/PathHelpers.h"
#include "../MainComponents/Graphics.h"

// Post Processes
#include "PostProcesses/BlurPostProcess.h"

namespace PostProcessManager
{
	void SetInitialTarget();
	void ResetRenderTarget();
	void Init();
	void OnResize();
	void DrawFinal();
	void DrawImGui();
	void AddPostProcess(std::shared_ptr<IPostProcessEffect> ppEffect);

};

