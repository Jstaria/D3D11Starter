int MaxParticles;

AppendStructuredBuffer<uint> DeadList : register(u0);

[numthreads(64, 1, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    if ((int) id.x >= MaxParticles)
        return;
    

}
