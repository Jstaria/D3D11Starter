
#include "../Header/DefaultParam.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.uv = (input.uv * uvScale) + uvOffset;

	float4 color = SurfaceColorTexture.Sample(BasicSampler, input.uv) * iTint;

	return color;
}