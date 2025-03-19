#include "../Header/DefaultParam.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.uv = (input.uv * uvScale) + uvOffset;
    
    float amplitude = .25;
    
    float wave = sin((input.uv.y) * 5 + iTime * 2) * amplitude;
    
    return float4(input.normal + 2 * wave, 0);

}