
#include "../Header/DefaultParam.hlsli"
#include "../Header/Lighting.hlsli"

Texture2D Albedo : register(t7);
Texture2D NormalMap : register(t8);
Texture2D RoughnessMap : register(t9);
Texture2D MetalnessMap : register(t10);

float4 main(VertexToPixel input) : SV_TARGET
{
    float shadowAmount = ShadowAmount(input);
    
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.uv = (input.uv * uvScale) + uvOffset;

    float3 totalLight = float3(0, 0, 0);
	
    float roughness = RoughnessMap.Sample(BasicSampler, input.uv).r;
    float metalness = MetalnessMap.Sample(BasicSampler, input.uv).r;
    
    float4 sampleColor = Albedo.Sample(BasicSampler, input.uv);
    float3 color = pow(sampleColor.xyz, 2.2f) * iTint.xyz;
	
    if (sampleColor.a == 0)
        discard;
    
        float3 normal = normalize(NormalMap.Sample(BasicSampler, input.uv).rgb * 2 - 1);
    
    // calc matrix
    float3 N = input.normal;
    float3 T = normalize(input.tangent - N * dot(N, input.tangent));
    float3 B = cross(T, N);
    
    float3x3 TBN = float3x3(T, B, N);
    input.normal = mul(normal, TBN);
    
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        Light light = lights[i];
        
        if (light.Intensity < 0)
            continue;

        
            switch (lights[i].Type)
    {
        case LIGHT_TYPE_DIRECTIONAL:
            totalLight += DirectionalLightPBR(
                lights[i], input, 
                color, roughness, metalness, iEyePosition) * (i == 0 ? shadowAmount : 1);
            break;
            
        case LIGHT_TYPE_POINT:
            totalLight += PointLightPBR(
                input, lights[i], color, roughness, metalness);
            break;
            
        case LIGHT_TYPE_SPOT:
            totalLight += SpotLightPBR(
                input, lights[i], color, roughness, metalness);
            break;
    }
    }
	
    float3 gammaAdjustedColor = pow(totalLight, 1.0f / 2.2f);
    
    return float4(gammaAdjustedColor, 1);
}
