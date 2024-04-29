#include "../Common.hlsli"

RWStructuredBuffer<Particle> particleBuffer : register(u1);


[numthreads(512, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID)
{
    uint particleIndex = DTid.x;
    Particle particle = particleBuffer[particleIndex];

    //float damping = 0.f;
    //float gravity = 9.8f;
    //particle.position += particle.velocity * DeltaTime;
    //particle.velocity += gravity * DeltaTime;
    //particle.velocity *= (1.0 - damping * DeltaTime);
    //particle.lifetime -= DeltaTime;


    //if (particle.lifetime <= 0.0)
    //{
    //    particle.position = float4(0, 0, 0, 1);
    //    particle.velocity = float4(0, 0, 0, 1);
    //    particle.lifetime = 10.0;
    //    particle.size = 0.1;
    //    particle.color = float4(0, 0, 1, 1);
    //}


    particleBuffer[particleIndex] = particle;
}