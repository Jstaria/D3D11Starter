#include "../Include/DefaultParam.hlsli"

float3 smoothRainbow(float x, float spread, float speed)
{
    float hue = frac(x * spread + iTime * speed); // Use continuous interpolation
    return hsv2rgb(float3(hue, 0.99f, 0.85f)); // Convert HSV to RGB
}

float4 main(VertexToPixel input) : SV_TARGET
{
    float2 uv = input.uv;
    
    float3 color = smoothRainbow(uv.y - uv.x, 2, .3);
    
    float first = .33;
    float second = .66;
    
    float3 color1 = float3(.75, .65, .65);
    float3 color2 = float3(.20, .20, .20);
    
    float offset = .05;
    
    float3 direction =  input.normal;
    
    float lerpDot = (1 + dot(normalize(direction), normalize(iEyeDirection))) * 1.75;
    
    color = lerp(color, iTint.rgb, lerpDot);
    
    if (uv.y < first)
    {
        float lerpValue = uv.y / first;
        
        color = lerp(color1, color2, lerpValue);
    }
    if (uv.y < first + offset / 2 && uv.y > first - offset)
    {
        color = color1 * .65;
    }
    if (uv.y > second)
    {
        float lerpValue = (uv.y - second) / first;
        
        color = lerp(color2, color1, lerpValue);
    }
    if (uv.y < second + offset && uv.y > second - offset / 2)
    {
        color = color1 * .65;
    }
    
        return float4(color, 1.0);
}