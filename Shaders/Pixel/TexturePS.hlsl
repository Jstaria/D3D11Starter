
#include "../Header/DefaultParam.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    input.uv = (input.uv * uvScale) + uvOffset;
    
    float4 color = SurfaceColorTexture.Sample(BasicSampler, input.uv);

    input.normal = normalize(input.normal);
    
    return color;
}