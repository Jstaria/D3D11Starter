#ifndef __LIGHTING_INCLUDES__ 
#define __LIGHTING_INCLUDES__

#define MAX_SPECULAR_EXPONENT 256.0f
#define MAX_LIGHTS 64

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

#include "../Header/DefaultParam.hlsli"

// RenderDoc for debugging

struct Light
{
    int Type; // Which kind of light? 0, 1 or 2 (see above)
    float3 Direction; // Directional and Spot lights need a direction
    
    float Range; // Point and Spot lights have a max range for attenuation
    float3 Position; // Point and Spot lights have a position in space
    
    float Intensity; // All lights need an intensity
    float3 Color; // All lights need a color

    float SpotInnerAngle; // Inner cone angle (in radians) – Inside this, full light!
    float SpotOuterAngle; // Outer cone angle (radians) – Outside this, no light!
    float2 Padding; // Purposefully padding to hit the 16-byte boundary
};

Light lights[MAX_LIGHTS];

float Attenuate(Light light, float3 worldPos)
{
    float dist = distance(light.Position, worldPos);
    float att = saturate(1.0f - (dist * dist / (light.Range * light.Range)));
    return att * att;
}

float3 LambertDiffuse(float3 lightDirection, float3 normal)
{
    // saturate(N*L)*C_surface*C_light*I_Light
    return saturate(dot(normal, lightDirection));
}

float PhongSpecular(float3 lightDirection, float3 normal, float3 worldPosition, float3 cameraPosition, float roughness)
{      
    float3 refl = reflect(-normalize(lightDirection), normal);
    
    float3 viewVector = normalize(cameraPosition - worldPosition);
    
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    float3 specTerm = pow(max(dot(viewVector, refl), 0.0f), specExponent);
    
    return roughness == 1 ? 0 : specTerm;
}

float3 DirectionalLight(Light light, float3 color, SamplerState BasicSampler, Texture2D SurfaceSpecularMap, VertexToPixel input)
{
    float3 lightDirection = normalize(-light.Direction);
    float lightIntensity = light.Intensity;
    float3 lightColor = light.Color;

    float3 diffuse = color * LambertDiffuse(lightDirection, input.normal);

    float3 spec = PhongSpecular(lightDirection, input.normal, input.worldPosition, iEyePosition,
        SurfaceSpecularMap.Sample(BasicSampler, input.uv).r);

    spec *= any(diffuse);
    
    return (diffuse * color + spec) * light.Intensity * light.Color;
}

float3 PointLight(Light light, float3 surfaceColor, SamplerState BasicSampler, Texture2D SurfaceSpecularMap, VertexToPixel input)
{
    float3 lightDirection = normalize(light.Position - input.worldPosition);
    float lightIntensity = light.Intensity;
    float3 lightColor = light.Color;

    float attenuation = Attenuate(light, input.worldPosition);
    
    float3 diffuse = light.Color * LambertDiffuse(lightDirection, input.normal);

    float3 spec = PhongSpecular(lightDirection, input.normal, input.worldPosition, iEyePosition,
        1 - SurfaceSpecularMap.Sample(BasicSampler, input.uv).r);

    spec *= any(diffuse);
    
    return (diffuse * surfaceColor + spec) * attenuation * lightIntensity * lightColor;
}

float3 SpotLight(Light light, float3 surfaceColor, SamplerState BasicSampler, Texture2D SurfaceSpecularMap, VertexToPixel input)
{
    float3 lightDirection = -normalize(light.Direction);
    float3 lightToPixel = normalize(light.Position - input.worldPosition);

    float pixelAngle = saturate(dot(lightToPixel, lightDirection));

    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);

    float spotTerm = saturate((cosOuter - pixelAngle) / (cosOuter - cosInner));
    
    return PointLight(light, surfaceColor, BasicSampler, SurfaceSpecularMap, input) * spotTerm;
}
#endif