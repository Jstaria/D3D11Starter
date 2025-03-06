
#include "../Header/DefaultParam.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color = SurfaceColorTexture.Sample(BasicSampler, input.uv);

    return color;
}