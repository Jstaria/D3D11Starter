#pragma once

#include <memory>
#include <d3d11.h>
#include <wrl/client.h>
#include <unordered_map> 

#include "../ImGui/imgui.h"
#include "../Helper/SimpleShader.h"
#include "../Helper/GlobalVar.h"
#include "../MainComponents/Window.h"
#include "../Components/Transform.h"
#include "../Structures/ExternalData.h"

class Material
{
private:
	std::shared_ptr<SimpleVertexShader> vs;
	std::shared_ptr<SimplePixelShader> ps;
	DirectX::XMFLOAT4 colorTint;
	DirectX::XMFLOAT3 ambientTint;
	const char* name;

	DirectX::XMFLOAT2 offset;
	DirectX::XMFLOAT2 scale;

	unsigned int materialIndex;

	std::unordered_map<const char*, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> textures;
	std::unordered_map<const char*, Microsoft::WRL::ComPtr<ID3D11SamplerState>> samplers;

public:
	Material(const char* name, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, DirectX::XMFLOAT4 color);

	void SetDefaultShaderParam(ExternalData data, Transform* transform, Transform* camTransform);

	// -=| Getters |=-
	std::shared_ptr<SimpleVertexShader> GetVS();
	std::shared_ptr<SimplePixelShader> GetPS();
	DirectX::XMFLOAT4 GetTintColor();
	DirectX::XMFLOAT3 GetAmbientColor();
	const char* GetName();
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> GetTextureSRV(const char* name);
	Microsoft::WRL::ComPtr<ID3D11SamplerState> GetSampler(const char* name);
	std::unordered_map<const char*, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> GetTextureSRVs();
	std::unordered_map<const char*, Microsoft::WRL::ComPtr<ID3D11SamplerState>> GetSamplers();
	unsigned int GetMatIndex();

	// -=| Setters/Adders |=-
	void AddTextureSRV(const char* name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV);
	void AddSampler(const char* name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler);
	void SetAmbientTint(DirectX::XMFLOAT3 ambientTint);
	void SetIndex();

	// -=| Removers |=-
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> RemoveTextureSRV(const char* name);
	Microsoft::WRL::ComPtr<ID3D11SamplerState> RemoveSampler(const char* name);

	// -=| Other |=-
	void ImGuiDraw();
};

