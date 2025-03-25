
#include "../Header/DefaultParam.hlsli"
#include "../Header/Lighting.hlsli"

float4 main(VertexToPixel input) : SV_TARGET
{
    input.normal = normalize(input.normal);
    input.tangent = normalize(input.tangent);
    input.uv = (input.uv * uvScale) + uvOffset;

    float3 totalLight = float3(0, 0, 0);
	
    float3 color = SurfaceColorTexture.Sample(BasicSampler, input.uv).xyz * iTint.xyz;
    
    float3 ambientTerm = iAmbientColor * color;
	
    float3 normalFromMap = normalize(SurfaceNormalMap.Sample(BasicSampler, input.uv) * 2 - 1).xyz;
    
    // calc matrix
    float3 N = input.normal;
    float3 T = normalize(input.tangent - N * dot(N, input.tangent));
    float3 B = cross(T, N);
    
    float3x3 TBN = float3x3(T, B, N);
    //input.normal = mul(normalFromMap, TBN);
    
    //return float4(normalFromMap, 1);
    
    totalLight += ambientTerm;
    
    for (int i = 0; i < MAX_LIGHTS; i++)
    {
        float3 lightDirection = lights[i].Direction;
        float lightIntensity = lights[i].Intensity;
        float3 lightColor = lights[i].Color;
        
        if (lightIntensity < 0)
            continue;
        
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight +=
                    (color * LambertDiffuse(lightDirection, lightColor, lightIntensity * 2, input.normal) +
                    PhongSpecular(
                        lightDirection, lightColor, lightIntensity,
                        input.normal, input.worldPosition, iEyePosition,
                        SurfaceSpecularMap.Sample(BasicSampler, input.uv).r)
                    );
                break;
        }
        
        
        
    }
	
    return float4(totalLight, 1);
}