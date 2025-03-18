
#include "Material.h"

using namespace Microsoft::WRL;

Material::Material(const char* name, std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, DirectX::XMFLOAT4 color)
	: name(name), vs(vs), ps(ps), color(color), scale(1,1), offset(0,0)
{
	materialIndex = GlobalVar::Material::getIndexThenTick();
}

void Material::SetDefaultShaderParam(ExternalData data, Transform* transform, Transform* camTransform)
{
	ps->SetFloat("iTime", GlobalVar::Time::getElapsedTime());
	ps->SetFloat2("iResolution", DirectX::XMFLOAT2((float)Window::Width(), (float)Window::Height()));
	ps->SetFloat3("iEyePosition", camTransform->GetPosition());
	ps->SetFloat3("iEyeDirection", camTransform->GetForward());
	ps->SetFloat3("iPosition", transform->GetPosition());
	
	vs->SetFloat3("iPosition", transform->GetPosition());
	vs->SetFloat("iTime", GlobalVar::Time::getElapsedTime());

	vs->SetData("data", &data, sizeof(ExternalData));
	ps->SetFloat4("iTint", color);

	ps->SetData("uvOffset", &offset, sizeof(DirectX::XMFLOAT2));
	ps->SetData("uvScale", &scale, sizeof(DirectX::XMFLOAT2));

	for (auto& textures : textures) { ps->SetShaderResourceView(textures.first, textures.second.Get()); }
	for (auto& samplers : samplers) { ps->SetSamplerState(samplers.first, samplers.second.Get()); }
}

std::shared_ptr<SimpleVertexShader> Material::GetVS() { return vs; }
std::shared_ptr<SimplePixelShader> Material::GetPS() { return ps; }
DirectX::XMFLOAT4 Material::GetColor() { return color; }
const char* Material::GetName() { return name; }

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::GetTextureSRV(const char* name) { return textures[name]; }
Microsoft::WRL::ComPtr<ID3D11SamplerState> Material::GetSampler(const char* name) { return samplers[name]; }
std::unordered_map<const char*, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>> Material::GetTextureSRVs() { return textures; }
std::unordered_map<const char*, Microsoft::WRL::ComPtr<ID3D11SamplerState>> Material::GetSamplers() { return samplers; }

unsigned int Material::GetMatIndex() { return materialIndex; }

void Material::AddTextureSRV(const char* name, Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureSRV) { textures.insert({ name, textureSRV }); }
void Material::AddSampler(const char* name, Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler) { samplers.insert({ name, sampler }); }

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> Material::RemoveTextureSRV(const char* name)
{
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> srv;
	std::memcpy(srv.GetAddressOf(), textures[name].GetAddressOf(), sizeof(ComPtr<ID3D11ShaderResourceView>));

	textures.erase(name);

	return srv;
}

Microsoft::WRL::ComPtr<ID3D11SamplerState> Material::RemoveSampler(const char* name)
{
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler;
	std::memcpy(sampler.GetAddressOf(), samplers[name].GetAddressOf(), sizeof(ComPtr<ID3D11SamplerState>));

	textures.erase(name);

	return sampler;
}

void Material::ImGuiDraw()
{
	ImGui::DragFloat2("UV Offset", &offset.x, .1f, -1, 1);
	ImGui::DragFloat2("UV Scale", &scale.x, .1f, 0, 5);
}
