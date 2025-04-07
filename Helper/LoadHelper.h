#pragma once

#include <string>
#include <memory>
#include <wrl/client.h>
#include <d3d11.h>
#include <WICTextureLoader.h>

#include "../MainComponents/Graphics.h"
#include "PathHelpers.h"

namespace LoadHelper {

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadTexture(const std::string address);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> CreateCubemap(const std::wstring path);

}