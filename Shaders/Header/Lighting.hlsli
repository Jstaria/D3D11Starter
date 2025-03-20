float3 LambertDiffuse(float3 lightDirection, float3 lightColor, float lightIntensity, float3 surfaceColor, float3 normal)
{
    // saturate(N*L)*C_surface*C_light*I_Light

    float3 diffuseTerm =
        max(dot(normal, -lightDirection), 0) *
        lightIntensity * lightColor * surfaceColor;
    
    return diffuseTerm;

}

float3 PhongSpecular(float3 lightDirection, float3 lightColor, float lightIntensity, float3 normal, float3 worldPosition, float3 cameraPosition)
{
    float3 refl = reflect(lightDirection, normal);
    
    float3 viewVector = normalize(cameraPosition - worldPosition);
    
    float3 specTerm = pow(max(dot(refl, viewVector), 0), 64) * lightIntensity * lightColor;
    
    return specTerm;
}