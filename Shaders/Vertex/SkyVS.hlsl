#include "../Header/DefaultParam.hlsli"

struct VertexToPixelSky
{
    float4 localPosition : SV_POSITION;
    float3 sampleDir : DIRECTION;
};

cbuffer ExternalData : register(b10)
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
}

VertexToPixelSky main(VertexShaderInput input)
{
    VertexToPixelSky output;
    
    float4x4 viewNoTranslation = viewMatrix;
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    float4x4 vp = mul(projMatrix, viewNoTranslation);
    
    output.localPosition = mul(vp, float4(input.localPosition, 1));
    output.localPosition.z = output.localPosition.w;
    output.sampleDir = input.localPosition.xyz;
    
	return output;
}