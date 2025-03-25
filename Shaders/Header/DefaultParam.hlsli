#ifndef DEFAULT_PARAM
#define DEFAULT_PARAM

#define PI 3.1415926535897932384626433832795
#define TAU 6.283185307179586476925286766559
#define HALF_PI 1.5707963267948966192313216916398

uniform float  iTime;
uniform float2 iResolution;
uniform float3 iEyePosition;
uniform float3 iEyeDirection;
uniform float4 iTint;
uniform float3 iPosition;
uniform float2 uvScale;
uniform float2 uvOffset;

Texture2D SurfaceColorTexture : register(t0);
Texture2D SurfaceNormalMap : register(t1);
Texture2D SurfaceSpecularMap : register(t2);
SamplerState BasicSampler : register(s0);

struct VertexToPixel
{
    float4 screenPosition : SV_POSITION;
    float2 uv : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 worldPosition : POSITION;
};

// Hash function from H. Schechter & R. Bridson, goo.gl/RXiKaH
uint Hash(uint s)
{
    s ^= 2747636419u;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    s ^= s >> 16;
    s *= 2654435769u;
    return s;
}

float Random(uint seed)
{
    return float(Hash(seed)) / 4294967295.0; // 2^32-1
}

// https://www.shadertoy.com/view/ldcyR4
// ===============================================================================================================
float3 hsv2rgb(float3 c)
{
    float3 rgb = clamp(abs(fmod(c.x * 6. + float3(0., 4., 2.), 6.) - 3.) - 1., 0., 1.);
    rgb = rgb * rgb * (3. - 2. * rgb);
    return c.z * lerp(float3(1, 1, 1), rgb, c.y);
}

float3 mod289(float3 x)
{
    return x - floor(x * (1 / 289)) * 289;
};

float2 mod289(float2 x)
{
    return x - floor(x * (1 / 289)) * 289;
};

float3 permute(float3 x)
{
    return mod289(((x * 34) + 1) * x);
};

float simplexNoise(float2 v)
{
    const float4 C = float4(.211324865405187, .366025403784439, -.577350269189626, .024390243902439);
    
    float2 i = floor(v + dot(v, C.yy));
    float2 x0 = v - i + dot(i, C.xx);
    float2 i1 = (x0.x > x0.y) ? float2(1., 0.) : float2(0., 1.);
    float4 x12 = x0.xyxy + C.xxzz;
    x12.xy -= i1;
    i = mod289(i.xy);
    
    float3 p = permute(permute(i.y + float3(0., i1.y, 1.)) + i.x + float3(0., i1.x, 1.));
    float3 m = max(0.5 - float3(dot(x0, x0), dot(x12.xy, x12.xy), dot(x12.zw, x12.zw)), 0.);
    m = m * m;
    m = m * m;
    float3 x = 2. * frac(p * C.www) - 1.;
    float3 h = abs(x) - 0.5;
    float3 ox = floor(x + 0.5);
    float3 a0 = x - ox;
    m *= 1.79284291400159 - .85373472095314 * (a0 * a0 + h * h);
    float3 g;
    g.x = a0.x * x0.x + h.x * x0.y;
    g.yz = a0.yz * x12.xz + h.yz * x12.yw;
    return 130. * dot(m, g);
};
// ===============================================================================================================
#endif // DEFAULT_PARAM