
#include "../Header/DefaultParam.hlsli"
#include "../Header/Lighting.hlsli"

float3 DirectionalLight(Light light, float3 color, SamplerState BasicSampler, Texture2D SurfaceSpecularMap, VertexToPixel input)
{
    float3 lightDirection = normalize(light.Direction);
    float lightIntensity = light.Intensity;
    float3 lightColor = light.Color;

    float3 diffuse = color * LambertDiffuse(lightDirection, lightColor, lightIntensity, input.normal);

    float3 spec = PhongSpecular(lightDirection, lightColor, lightIntensity, input.normal, input.worldPosition, iEyePosition,
        SurfaceSpecularMap.Sample(BasicSampler, input.uv).r);

    return diffuse + spec;
}

float3 PointLight(Light light, float3 color, SamplerState BasicSampler, Texture2D SurfaceSpecularMap, VertexToPixel input)
{
    float3 lightDirection = normalize(input.worldPosition - light.Position);
    float lightIntensity = light.Intensity;
    float3 lightColor = light.Color;

    float attenuation = Attenuate(light, input.worldPosition);
    
    float3 diffuse = color * LambertDiffuse(lightDirection, lightColor, lightIntensity, input.normal);

    float3 spec = PhongSpecular(lightDirection, lightColor, lightIntensity, input.normal, input.worldPosition, iEyePosition,
        1 - SurfaceSpecularMap.Sample(BasicSampler, input.uv).r);

    return (diffuse + spec) * attenuation;
}

float3 SpotLight(Light light, float3 color, SamplerState BasicSampler, Texture2D SurfaceSpecularMap, VertexToPixel input)
{
    float3 lightDirection = normalize(light.Direction);
    float3 lightToPixel = normalize(input.worldPosition - light.Position);

    float pixelAngle = saturate(dot(lightToPixel, lightDirection));

    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);
    float falloffRange = cosOuter - cosInner;

    float spotTerm = saturate((cosOuter - pixelAngle) / falloffRange);

    return PointLight(light, color, BasicSampler, SurfaceSpecularMap, input) * spotTerm;
}

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
        if (lights[i].Intensity < 0)
            continue;
        
        switch (lights[i].Type)
        {
            case LIGHT_TYPE_DIRECTIONAL:
                totalLight += DirectionalLight(lights[i], color, BasicSampler, SurfaceSpecularMap, input);
                break;
            
            case LIGHT_TYPE_POINT:
                totalLight += PointLight(lights[i], color, BasicSampler, SurfaceSpecularMap, input);
                break;
            
            case LIGHT_TYPE_SPOT:
                totalLight += SpotLight(lights[i], color, BasicSampler, SurfaceSpecularMap, input);
                break;
        }
        
        
        
    }
	
    return float4(totalLight, 1);
}
