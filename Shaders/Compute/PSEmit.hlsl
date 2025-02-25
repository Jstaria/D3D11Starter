

struct Particle
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

RWStructuredBuffer<Particle> ParticlePool : register(u0);
ConsumeStructuredBuffer<uint> DeadList : register(u1);

cbuffer DeadListCounterBuffer : register(b1) {
    uint DeadListCounter;
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= (uint)p_Emitter.e_MaxParticles || DeadListCounter == 0 || id.x >= DeadListCounter) 
        return;

    uint emitIndex = DeadList.Consume();
    
    Particle p_EmitParticle = ParticlePool.Load(emitIndex);

    p_EmitParticle.p_Mass = p_Emitter.e_Mass;
    p_EmitParticle.p_TrVelocity = p_Emitter.e_EmitterVelocity;
    p_EmitParticle.p_LifeSpan = p_Emitter.e_ParticleLifeSpan;
    p_EmitParticle.p_Age = p_EmitParticle.p_LifeSpan;

    ParticlePool[emitIndex] = p_EmitParticle;
}
