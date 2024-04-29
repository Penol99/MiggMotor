#include "FFTComputeCommon.hlsli"

RWTexture2D<float4> PrecomputeBuffer : register(u0);

[numthreads(1, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint step = id.x;
    uint index = id.y;
    uint halfSize = Size / 2;

    uint b = Size >> (step + 1);
    float2 mult = 2 * PI * float2(0, 1) / Size;
    uint i = (2 * b * (index / b) + index % b) % Size;
    float2 twiddle = ComplexExp(-mult * ((index / b) * b));

    PrecomputeBuffer[uint2(step, index)] = float4(twiddle.x, twiddle.y, i, i + b);
    PrecomputeBuffer[uint2(step, index + halfSize)] = float4(-twiddle.x, -twiddle.y, i, i + b);
}