#ifndef __LIGHTING_INCLUDES__ 
#define __LIGHTING_INCLUDES__

#define MAX_SPECULAR_EXPONENT 256.0f
#define MAX_LIGHTS 64

#define LIGHT_TYPE_DIRECTIONAL 0
#define LIGHT_TYPE_POINT 1
#define LIGHT_TYPE_SPOT 2

#include "../Header/DefaultParam.hlsli"

// RenderDoc for debugging

static const float F0_NON_METAL = 0.04f;

// Need a minimum roughness for when spec distribution function denominator goes to zero
#define MIN_ROUGHNESS 0.0000001 // 6 zeros after 
#define TWO_PI PI * 2.0f
#define QUARTER_PI PI / 4.0f




// PBR FUNCTIONS ================

// Lambert diffuse BRDF - Same as the basic lighting diffuse calculation!
// - NOTE: this function assumes the vectors are already NORMALIZED!
float DiffusePBR(float3 normal, float3 dirToLight)
{
    return saturate(dot(normal, dirToLight));
}



// Calculates diffuse amount based on energy conservation
//
// diffuse   - Diffuse amount
// F         - Fresnel result from microfacet BRDF
// metalness - surface metalness amount 
float3 DiffuseEnergyConserve(float3 diffuse, float3 F, float metalness)
{
    return diffuse * (1 - F) * (1 - metalness);
}


// Normal Distribution Function: GGX (Trowbridge-Reitz)
//
// a - Roughness
// h - Half vector
// n - Normal
// 
// D(h, n, a) = a^2 / pi * ((n dot h)^2 * (a^2 - 1) + 1)^2
float D_GGX(float3 n, float3 h, float roughness)
{
	// Pre-calculations
    float NdotH = saturate(dot(n, h));
    float NdotH2 = NdotH * NdotH;
    float a = roughness * roughness;
    float a2 = max(a * a, MIN_ROUGHNESS); // Applied after remap!

	// ((n dot h)^2 * (a^2 - 1) + 1)
	// Can go to zero if roughness is 0 and NdotH is 1
    float denomToSquare = NdotH2 * (a2 - 1) + 1;

	// Final value
    return a2 / (PI * denomToSquare * denomToSquare);
}



// Fresnel term - Schlick approx.
// 
// v - View vector
// h - Half vector
// f0 - Value when l = n
//
// F(v,h,f0) = f0 + (1-f0)(1 - (v dot h))^5
float3 F_Schlick(float3 v, float3 h, float3 f0)
{
	// Pre-calculations
    float VdotH = saturate(dot(v, h));

	// Final value
    return f0 + (1 - f0) * pow(1 - VdotH, 5);
}

// Geometric Shadowing - Schlick-GGX
// - k is remapped to a / 2, roughness remapped to (r+1)/2 before squaring!
//
// n - Normal
// v - View vector
//
// G_Schlick(n,v,a) = (n dot v) / ((n dot v) * (1 - k) * k)
//
// Full G(n,v,l,a) term = G_SchlickGGX(n,v,a) * G_SchlickGGX(n,l,a)
float G_SchlickGGX(float3 n, float3 v, float roughness)
{
	// End result of remapping:
    float k = pow(roughness + 1, 2) / 8.0f;
    float NdotV = saturate(dot(n, v));

	// Final value
	// Note: Numerator should be NdotV (or NdotL depending on parameters).
	// However, these are also in the BRDF's denominator, so they'll cancel!
	// We're leaving them out here AND in the BRDF function as the
	// dot products can get VERY small and cause rounding errors.
    return 1 / (NdotV * (1 - k) + k);
}


 
// Cook-Torrance Microfacet BRDF (Specular)
//
// f(l,v) = D(h)F(v,h)G(l,v,h) / 4(n dot l)(n dot v)
// - parts of the denominator are canceled out by numerator (see below)
//
// D() - Normal Distribution Function - Trowbridge-Reitz (GGX)
// F() - Fresnel - Schlick approx
// G() - Geometric Shadowing - Schlick-GGX
float3 MicrofacetBRDF(float3 n, float3 l, float3 v, float roughness, float3 f0, out float3 F_out)
{
	// Other vectors
    float3 h = normalize(v + l);

	// Run numerator functions
    float D = D_GGX(n, h, roughness);
    float3 F = F_Schlick(v, h, f0);
    float G = G_SchlickGGX(n, v, roughness) * G_SchlickGGX(n, l, roughness);
	
	// Pass F out of the function for diffuse balance
    F_out = F;

	// Final specular formula
	// Note: The denominator SHOULD contain (NdotV)(NdotL), but they'd be
	// canceled out by our G() term.  As such, they have been removed
	// from BOTH places to prevent floating point rounding errors.
    float3 specularResult = (D * F * G) / 4;

	// One last non-obvious requirement: According to the rendering equation,
	// specular must have the same NdotL applied as diffuse!  We'll apply
	// that here so that minimal changes are required elsewhere.
    return specularResult * max(dot(n, l), 0);
}



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
    
    float specTerm = pow(max(dot(viewVector, refl), 0.0f), specExponent);
    
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
        SurfaceSpecularMap.Sample(BasicSampler, input.uv).r);

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

float3 DirectionalLightPBR(Light light, VertexToPixel input, float3 albedo, float roughness, float metalness, float3 eyePos)
{
    float3 specColor = lerp(F0_NON_METAL, albedo.rgb, metalness);
    float3 toLight = normalize(-light.Direction);
    float3 toCam = normalize(iEyePosition - input.worldPosition);
        
        // Calculate the light amounts
    float diff = DiffusePBR(input.normal, toLight);
    float3 F;
    float3 spec = MicrofacetBRDF(input.normal, toLight, toCam, roughness, specColor, F);
        // Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness);
        // Combine the final diffuse and specular values for this light
    return (balancedDiff * albedo + spec) * light.Intensity * light.Color;
}

float3 PointLightPBR(VertexToPixel input, Light light, float3 albedo, float roughness, float metalness)
{
    float3 specColor = lerp(F0_NON_METAL, albedo.rgb, metalness);
    float3 toLight = normalize(light.Position - input.worldPosition);
    float3 toCam = normalize(iEyePosition - input.worldPosition);
        
    
    float atten = Attenuate(light, input.worldPosition);
        // Calculate the light amounts
    float diff = DiffusePBR(input.normal, toLight);
    float3 F;
    float3 spec = MicrofacetBRDF(input.normal, toLight, toCam, roughness, specColor, F);
        // Calculate diffuse with energy conservation, including cutting diffuse for metals
    float3 balancedDiff = DiffuseEnergyConserve(diff, F, metalness) * albedo;
        // Combine the final diffuse and specular values for this light
    return (balancedDiff * albedo + spec) * light.Intensity * light.Color * atten;
}

float3 SpotLightPBR(VertexToPixel input, Light light, float3 albedo, float roughness, float metalness)
{
    float3 lightDirection = -normalize(light.Direction);
    float3 lightToPixel = normalize(light.Position - input.worldPosition);

    float pixelAngle = saturate(dot(lightToPixel, lightDirection));

    float cosOuter = cos(light.SpotOuterAngle);
    float cosInner = cos(light.SpotInnerAngle);

    float spotTerm = saturate((cosOuter - pixelAngle) / (cosOuter - cosInner));
    
    return PointLightPBR(input, light, albedo, roughness, metalness) * spotTerm;
}
#endif