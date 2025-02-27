
#include "Material.h"

Material::Material(std::shared_ptr<SimpleVertexShader> vs, std::shared_ptr<SimplePixelShader> ps, DirectX::XMFLOAT4 color)
	: vs(vs), ps(ps), color(color)
{
}

std::shared_ptr<SimpleVertexShader> Material::GetVS()
{
	return vs;
}

std::shared_ptr<SimplePixelShader> Material::GetPS()
{
	return ps;
}

DirectX::XMFLOAT4 Material::GetColor()
{
	return color;
}
