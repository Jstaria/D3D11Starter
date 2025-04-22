#ifndef DEFAULT_PARAM
#define DEFAULT_PARAM

// ===============================================================================================================

#define PI 3.1415926535897932384626433832795
#define TAU 6.283185307179586476925286766559
#define HALF_PI 1.5707963267948966192313216916398

uniform float4 iTint;
uniform float2 uvScale;
uniform float2 uvOffset;
uniform float iTime;
uniform float3 iAmbientColor;
uniform float3 iEyePosition;
uniform float3 iEyeDirection;
uniform float3 iPosition;
uniform float2 iResolution;

Texture2D SurfaceColorTexture : register(t1);
Texture2D SurfaceNormalMap : register(t2);
Texture2D SurfaceSpecularMap : register(t3);
Texture2D ShadowMap : register(t4);

SamplerState BasicSampler : register(s0);
SamplerComparisonState ShadowSampler : register(s1);

struct VertexShaderInput
{
    float3 localPosition : POSITION; 
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
};

struct ExternalData
{
    float4x4 viewMatrix;
    float4x4 projMatrix;
    float4x4 worldMatrix;
    float4x4 invWorldMatrix;
};

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 worldPosition : POSITION;
    float4 shadowMapPos : SHADOW_POSITION;
};
// ===============================================================================================================
#endif // DEFAULT_PARAM