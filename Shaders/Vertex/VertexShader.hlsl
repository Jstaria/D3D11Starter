#include "../Header/DefaultParam.hlsli"

// Struct representing a single vertex worth of data
// - This should match the vertex definition in our C++ code
// - By "match", I mean the size, order and number of members
// - The name of the struct itself is unimportant, but should be descriptive
// - Each variable must have a semantic, which defines its usage
struct VertexShaderInput
{ 
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float3 localPosition	: POSITION;     // XYZ position
    float2 uv               : TEXCOORD;
    float3 normal           : NORMAL;
    float3 tangent          : TANGENT;
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
	
    output.worldPosition = mul(data.worldMatrix, float4(input.localPosition, 1.0f)).xyz;
    output.screenPosition = mul(wvp, float4(input.localPosition, 1.0f));
    output.uv = input.uv;
    output.normal = normalize(mul((float3x3) data.invWorldMatrix, input.normal));
    output.tangent = normalize(mul((float3x3) data.invWorldMatrix, input.tangent));
    return output;
}