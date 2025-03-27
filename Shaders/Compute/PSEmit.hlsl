
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

struct Emitter
{
    float4 e_EmitterPosition;
    float4 e_EmitterVelocity;
    float4 e_PositionVariance;
    
    int    e_MaxParticles;
    float  e_ParticleLifeSpan;
    float2 e_StartEndSize;
    
    float4 e_AppliedForce;
    float4 e_RotVelocity;
    
    float  e_VelocityVariance;
    float  e_Mass;
};

Emitter p_Emitter;

RWStructuredBuffer<DrawParticle> ParticleDrawPool : register(u0);
RWStructuredBuffer<ParticleUpdate> ParticleUpdatePool : register(u1);
ConsumeStructuredBuffer<uint> DeadList : register(u2);

cbuffer DeadListCounterBuffer : register(b1) {
    uint DeadListCounter;
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    // Check if index is within number of available particles to emit
    if (id.x >= (uint)p_Emitter.e_MaxParticles || DeadListCounter == 0 || id.x >= DeadListCounter) 
        return;

    // Grab an index from the dead particle list (DeadList starts off full when initialized)
    uint emitIndex = DeadList.Consume();
    
    // Update particle using DeadList index
    // ============================================================================
    
    // Set the particle half that will be used for updating
    // ------------------------------------------------------------------
    ParticleUpdate p_UpdateParticle = ParticleUpdatePool.Load(emitIndex);

    p_UpdateParticle.p_Mass = p_Emitter.e_Mass;
    p_UpdateParticle.p_TrVelocity = p_Emitter.e_EmitterVelocity.xyz;
    p_UpdateParticle.p_LifeSpan = p_Emitter.e_ParticleLifeSpan;
    p_UpdateParticle.p_Age = p_UpdateParticle.p_LifeSpan;

    ParticleUpdatePool[emitIndex] = p_UpdateParticle;
    // ------------------------------------------------------------------
    
    
    // Set the other half of the particle that will be used for rendering
    // ------------------------------------------------------------------
    DrawParticle p_DrawParticle = ParticleDrawPool.Load(emitIndex);
    
    p_DrawParticle.p_Position = p_Emitter.e_EmitterPosition;
    p_DrawParticle.p_Rotation = float4(0, 0, 0, 0);
    p_DrawParticle.p_Scale = p_Emitter.e_StartEndSize.xxxx;
    
    ParticleDrawPool[emitIndex] = p_DrawParticle;
    // ------------------------------------------------------------------
    
    // ============================================================================
}
