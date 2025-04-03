#include "../Header/DefaultParam.hlsli"

struct VertexToPixelSky
{
    float4 position : POSITION;
    float3 sampleDir : DIRECTION;
};

TextureCube CubeMap : register(t0);

float4 main(VertexToPixelSky input) : SV_TARGET
{
    return CubeMap.Sample(BasicSampler, normalize(input.sampleDir));
}