#ifndef __LIGHTING_INCLUDES__ 
#define __LIGHTING_INCLUDES__

struct Light
{
    
};

float3 LambertDiffuse(float3 lightDirection, float3 lightColor, float lightIntensity, float3 surfaceColor, float3 normal)
{
    // saturate(N*L)*C_surface*C_light*I_Light

    float3 diffuseTerm =
        max(dot(normal, -lightDirection), 0) *
        lightIntensity * lightColor * surfaceColor;
    
    return diffuseTerm;

}

// TODO: Add specular map
float3 PhongSpecular(float3 lightDirection, float3 lightColor, float lightIntensity, float3 normal, float3 worldPosition, float3 cameraPosition, float roughness)
{
    float3 refl = reflect(lightDirection, normal);
    
    float3 viewVector = normalize(cameraPosition - worldPosition);
    
    float specExponent = (1.0f - roughness) * 128;
    
    float3 specTerm = pow(max(dot(refl, viewVector), 0), specExponent) * lightIntensity * lightColor;
    
    return specTerm;
}
#endif