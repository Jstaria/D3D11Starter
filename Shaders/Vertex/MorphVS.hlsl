#include "../Header/DefaultParam.hlsli"

struct VertexShaderInput
{
    float3 localPosition : POSITION; // XYZ position
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
};

struct ExternalData
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 worldMatrix;
    float4x4 invWorldMatrix;
};

ExternalData data;

VertexToPixel main(VertexShaderInput input) 
{
	// Set up output struct
    VertexToPixel output;

    float4x4 wvp = mul(data.projMatrix, mul(data.viewMatrix, data.worldMatrix));
	
    float amplitude = .25;
    
    float wave = sin((input.uv.y) * 5 + iTime * 2) * amplitude;
    
    input.localPosition += normalize(input.localPosition - iPosition) * wave;
    
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.uv = input.uv;
    output.normal = mul(input.normal, (float3x3)data.invWorldMatrix);
    output.normal = normalize(output.normal);

    return output;
}