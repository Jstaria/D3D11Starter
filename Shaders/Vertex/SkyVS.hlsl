#include "../Header/DefaultParam.hlsli"

struct VertexToPixelSky
{
    float4 position : POSITION;
    float3 sampleDir : DIRECTION;
};

cbuffer ExternalData : register(b0)
{
    matrix viewMatrix;
    matrix projMatrix;
}

VertexToPixelSky main(VertexShaderInput input)
{
    VertexToPixelSky output;
    
    float4x4 viewNoTranslation = viewMatrix;
    float4x4 vp = mul(projMatrix, viewMatrix);
    
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    output.position = mul(vp, float4(input.localPosition, 1)).xyzz;
    output.sampleDir = normalize(output.position.xyz);
    
	return output;
}