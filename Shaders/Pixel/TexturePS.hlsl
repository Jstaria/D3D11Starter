
#include "../Header/DefaultParam.hlsli"

Texture2D ColorTexture : register(t0);
SamplerState BasicSampler : register(s0);

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color = ColorTexture.Sample(BasicSampler, input.uv);

    return color;
}