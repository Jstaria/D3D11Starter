#include "../Header/DefaultParam.hlsli"

struct VertexToPixelSky
{
    float4 position : POSITION;
    float3 sampleDir : DIRECTION;
};

ExternalData data;

VertexToPixelSky main(VertexShaderInput input)
{
    VertexToPixelSky output;
    
    float4x4 viewNoTranslation = data.viewMatrix;
    float4x4 vp = mul(data.projMatrix, data.viewMatrix);
    
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    output.position = mul(vp, float4(input.localPosition, 1)).xyzz;
    output.sampleDir = normalize(output.position.xyz);
    
	return output;
}