
#include "../Header/DefaultParam.hlsli"
#include "../Header/Lighting.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    float shadowAmount = ShadowAmount(input);

    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.uv = (input.uv * uvScale) + uvOffset;

    float3 totalLight = float3(0, 0, 0);
	
    float3 color = SurfaceColorTexture.Sample(BasicSampler, input.uv).xyz * iTint.xyz;
    color = pow(color, 2.2f);
    
    float3 ambientTerm = iAmbientColor * color;
	
    float3 normalFromMap = normalize(SurfaceNormalMap.Sample(BasicSampler, input.uv) * 2 - 1).xyz;
    
    // calc matrix
    float3 N = input.normal;
    float3 T = normalize(input.tangent - N * dot(N, input.tangent));
    float3 B = cross(T, N);
    
    float3x3 TBN = float3x3(T, B, N);
    input.normal = mul(normalFromMap, TBN);
    
    totalLight += ambientTerm;
    
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        if (lights[i].Intensity < 0)
            continue;
        
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionalLight(lights[i], color, BasicSampler, SurfaceSpecularMap, input) * (i == 0 ? shadowAmount : 1);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(lights[i], color, BasicSampler, SurfaceSpecularMap, input);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(lights[i], color, BasicSampler, SurfaceSpecularMap, input);
                break;
        }
    }
	
    float3 gammaAdjustedColor = pow(totalLight, 1.0f / 2.2f);
    
    return float4(gammaAdjustedColor, 1);
}
