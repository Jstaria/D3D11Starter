
#include "../Header/DefaultParam.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float4 color = SurfaceColorTexture.Sample(BasicSampler, input.uv);

    input.normal = normalize(input.normal);
    return float4(input.normal.xyz, 1);
    
    return color;
}