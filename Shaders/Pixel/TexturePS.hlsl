
#include "../Header/DefaultParam.hlsli"
#include "../Header/Lighting.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
	input.uv = (input.uv * uvScale) + uvOffset;

    float totalLight = float3(0, 0, 0);
	
    float3 lightDirection = float3(1, -.25f, 0);
    float3 lightColor = float3(1, 1, 1);
    float lightIntensity = 1.0f;
	
    float3 color = SurfaceColorTexture.Sample(BasicSampler, input.uv) * iTint;
    float3 ambientColor = float3(0.1, 0.1, 0.1);
    float3 ambientTerm = ambientColor * color;
	
    totalLight += 
    ambientTerm + 
    LambertDiffuse(lightDirection, lightColor, lightIntensity, color, input.normal) +
    PhongSpecular(lightDirection, lightColor, lightIntensity, input.normal, input.worldPosition, iEyePosition);
	
	return totalLight;
}