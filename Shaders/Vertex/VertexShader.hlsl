#include "../Header/DefaultParam.hlsli"

ExternalData data;

VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
    VertexToPixel output;

    float4x4 wvp = mul(data.projMatrix, mul(data.viewMatrix, data.worldMatrix));
	
    output.worldPosition = mul(data.worldMatrix, float4(input.localPosition, 1.0f)).xyz;
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.uv = input.uv;
    output.normal = normalize(mul((float3x3) data.invWorldMatrix, input.normal));
    output.tangent = normalize(mul((float3x3) data.invWorldMatrix, input.tangent));
    return output;
}