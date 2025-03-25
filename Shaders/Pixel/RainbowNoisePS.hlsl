// Shadertoy link
// This was a pain to debug and convert, good learning experience
// https://www.shadertoy.com/view/ldcyR4
#include "../Header/DefaultParam.hlsli"
#include "../Header/Rainbow.hlsli"

float4 tint;

float2 pq(float2 uv)
{
    return float2(atan2(uv.x, uv.y) / TAU + .5, length(uv));;
}

float4 glorb(float2 uv, float2 offset, float radius)
{
    float2 pq1 = pq(uv + offset);
    float r = radius * simplexNoise(uv + iTime * .2);
    float s = 8. / iResolution.y;
    float m = smoothstep(r + s, r - s, pq1.y);
    float3 c = hsv2rgb(float3(pq1.x, 1., 1.));
    return float4(c, 1.) * m;
}

float4 field(float2 uv, float2 offset, float radius)
{
    float4 c0 = glorb(uv, offset, radius);
    float4 c1 = glorb(uv, offset, radius * .92);
    return c0 - c1;
}

float4 main(VertexToPixel input) : SV_TARGET
{
    input.uv = (input.uv * uvScale) + uvOffset;
    
    float snV = .5;
    
    float2 uv = input.uv;
    float4 r0 = field(uv, float2(.0, .0), 1.66);
    float4 r1 = field(uv, float2(snV, snV), 1.66);
    float4 r2 = field(uv, float2(snV, -snV), 1.66);
    float4 r3 = field(uv, float2(-snV, -snV), 1.66);
    float4 r4 = field(uv, float2(-snV, snV), 1.66);
    
    return r0 + r1 + r2 + r3 + r4;
}