#ifndef __LIGHTING_INCLUDES__ 
#define __LIGHTING_INCLUDES__

#define MAX_SPECULAR_EXPONENT 32.0f
#define MAX_LIGHTS 64

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

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

float3 LambertDiffuse(float3 lightDirection, float3 lightColor, float lightIntensity, float3 normal)
{
    // saturate(N*L)*C_surface*C_light*I_Light

    float3 diffuseTerm =
        max(dot(normal, -lightDirection), 0) *
        lightIntensity * lightColor;
    
    return diffuseTerm;

}

// TODO: Add specular map
float3 PhongSpecular(float3 lightDirection, float3 lightColor, float lightIntensity, float3 normal, float3 worldPosition, float3 cameraPosition, float roughness)
{
    float3 refl = reflect(lightDirection, normal);
    
    float3 viewVector = normalize(cameraPosition - worldPosition);
    
    float specExponent = (1.0f - roughness) * MAX_SPECULAR_EXPONENT;
    
    float3 specTerm = pow(max(dot(refl, viewVector), 0.0f), specExponent) * lightIntensity * lightColor;
    
    return specTerm;
}
#endif