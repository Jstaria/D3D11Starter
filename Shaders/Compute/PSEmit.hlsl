
struct Particle
{
    float4 trVelFric; 
    float4 rotVelFric;
    float4 scaleVelFric;
    float4 appliedForceMass;
    float3 position;
    float3 rotation;
    float3 scale;
};

int EmitCount;

RWStructuredBuffer<Particle> ParticlePool : register(u0);
ConsumeStructuredBuffer<uint> DeadList : register(u1);

cbuffer DeadListCounterBuffer : register(b1) {
    uint DeadListCounter;
}

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if (id.x >= (uint)EmitCount || DeadListCounter == 0 || id.x >= DeadListCounter) 
        return;

    uint emitIndex = DeadList.Consume();
    
    Particle emitParticle = ParticlePool.Load(emitIndex);

    // <Update Particle>

    ParticlePool[emitIndex] = emitParticle;
}
