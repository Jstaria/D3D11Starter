
#include "../Header/DefaultParam.hlsli"
#include "../Header/Lighting.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
	input.uv = (input.uv * uvScale) + uvOffset;

    float3 totalLight = float3(0, 0, 0);
	
    float3 lightDirection = float3(1, -.25f, 0);
    float3 lightColor = float3(1, 1, 1);
    float lightIntensity = .5f;
	
    float3 color = SurfaceColorTexture.Sample(BasicSampler, input.uv).xyz * iTint.xyz;
    float3 ambientColor = float3(0.1, 0.1, 0.1);
    float3 ambientTerm = ambientColor * color;
	
    float3 normalFromMap = normalize(SurfaceNormalMap.Sample(BasicSampler, input.uv) * 2 - 1).xyz;
    
    // calc matrix
    float3 N = input.normal;
    float3 T = normalize(input.tangent - N * dot(N, input.tangent));
    float3 B = cross(T,N);
    
    float3x3 TBN = float3x3(T, B, N);
    //input.normal = mul(normalFromMap, TBN);
    
    //return float4(normalFromMap, 1);
    
    totalLight += 
    ambientTerm + 
    (color * LambertDiffuse(lightDirection, lightColor, lightIntensity * 2, input.normal) +
    PhongSpecular(lightDirection, lightColor, lightIntensity / 2, input.normal, input.worldPosition, iEyePosition, 1 - SurfaceSpecularMap.Sample(BasicSampler, input.uv).r));
	
    return float4(totalLight, 1);
}