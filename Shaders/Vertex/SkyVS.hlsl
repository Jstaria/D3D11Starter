#include "../Header/DefaultParam.hlsli"

struct VertexToPixelSky
{
    float4 position;
    float3 sampleDir;
};

ExternalData data;

float4 main(VertexShaderInput input)
{
    VertexToPixelSky output;
    
    float4x4 viewNoTranslation = data.viewMatrix;
    float4x4 vp = mul(data.projMatrix, data.viewMatrix);
    
    viewNoTranslation._14 = 0;
    viewNoTranslation._24 = 0;
    viewNoTranslation._34 = 0;
    
    output.position = mul(vp, float4(input.localPosition, 1)).xyzz;
    output.sampleDir = output.position;
    
	return output;
}