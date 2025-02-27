struct ParticleUpdate
{
    float3 p_TrVelocity;
    float1 p_friction;
    
    float3 p_RotVelocity;
    float1 p_RotFriction;
    
    float2 p_ScaleStartEnd;
    float1 p_LifeSpan;
    float1 p_Age;
    
    float3 p_AppliedForce;
    float1 p_Mass;
};

struct DrawParticle
{
    float4 p_Position;
    float4 p_Rotation;
    float4 p_Scale;
};

int MaxParticles;

RWStructuredBuffer<DrawParticle> ParticleDrawPool : register(u0);
RWStructuredBuffer<ParticleUpdate> ParticleUpdatePool : register(u1);
AppendStructuredBuffer<uint> DeadList : register(u2);

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{

}
