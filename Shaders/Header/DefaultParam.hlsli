#ifndef DEFAULT_PARAM
#define DEFAULT_PARAM

// ===============================================================================================================

#define PI 3.1415926535897932384626433832795
#define TAU 6.283185307179586476925286766559
#define HALF_PI 1.5707963267948966192313216916398

cbuffer GenericData : register(b1)
{
    float4 iTint;
    float2 uvScale;
    float2 uvOffset;
    float iTime;
    float3 iAmbientColor;
    float3 iEyePosition;
    float3 iEyeDirection;
    float3 iPosition;
    float2 iResolution;
};

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