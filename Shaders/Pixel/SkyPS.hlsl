#include "../Header/DefaultParam.hlsli"

struct VertexToPixelSky
{
    float4 position : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

TextureCube CubeMap : register(t0);
SamplerState SkySampler : register(s1);

float4 main(VertexToPixelSky input) : SV_TARGET
{
    return CubeMap.Sample(SkySampler, input.sampleDir);
}