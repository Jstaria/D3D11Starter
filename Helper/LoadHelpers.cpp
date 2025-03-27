#include "LoadHelper.h"

Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> LoadHelper::LoadTexture(const std::string address) {
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> SRV;
	DirectX::CreateWICTextureFromFile(Graphics::Device.Get(), Graphics::Context.Get(),
		FixPath(NarrowToWide(address)).c_str(), 0, SRV.GetAddressOf());

	return SRV;
}