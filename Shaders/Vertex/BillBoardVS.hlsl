#include "../Header/DefaultParam.hlsli"

ExternalData data;

struct VertexInput
{
    float3 position : POSITION;
    float2 uv : TEXCOORD;
    uint unitID : UNITID;
};

cbuffer lightData : register(b4)
{
    Matrix lightProjection;
    Matrix lightView;
}

float4x4 RemoveWorldMatrixRotation(float4x4 worldMatrix)
{
    // Extract scale from each axis
    float3 scale = float3(
        length(worldMatrix._11_12_13), // X scale
        length(worldMatrix._21_22_23), // Y scale
        length(worldMatrix._31_32_33) // Z scale
    );

    // Extract translation (last column)
    float4 translation = worldMatrix._14_24_34_44;

    // Rebuild matrix with zero rotation (identity rotation)
    return float4x4(
        scale.x, 0, 0, translation.x,
        0, scale.y, 0, translation.y,
        0, 0, scale.z, translation.z,
        0, 0, 0, translation.w
    );
}

VertexToPixel main(VertexInput input)
{
	// Set up output struct
    VertexToPixel output;
    
    float4x4 worldNoRotation = RemoveWorldMatrixRotation(data.worldMatrix);
    float4x4 wvp = mul(data.projMatrix, mul(data.viewMatrix, worldNoRotation));
	
    float3 cameraUp = float3(0, 1, 0);
    
    float3 look = normalize(iEyeDirection); //-normalize(iEyePosition - mul(data.worldMatrix, float4(input.position, 1.0f)).xyz);
    float3 right = normalize(cross(cameraUp, look));
    float3 up = cross(look, right);
    
    float2 offsets[4] =
    {
        float2(-1, 1), 
        float2(1, 1), 
        float2(-1, -1), 
        float2(1, -1)
    };
    
    float2 offset = offsets[input.unitID];
    
    float3 worldPosition = input.position + (right * offset.x) + (up * offset.y);
    
    output.worldPosition = mul(data.worldMatrix, float4(worldPosition, 1.0f)).xyz;
    output.screenPosition = mul(wvp, float4(worldPosition, 1.0f));
    output.uv = input.uv;
    output.normal = look;
    output.tangent = right;
    
    matrix shadowWVP = mul(lightProjection, mul(lightView, data.worldMatrix));
    output.shadowMapPos = mul(shadowWVP, float4(worldPosition, 1.0f));
    
    return output;
}