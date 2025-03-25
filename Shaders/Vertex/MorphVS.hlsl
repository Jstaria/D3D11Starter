#include "../Header/DefaultParam.hlsli"

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