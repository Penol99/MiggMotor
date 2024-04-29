#include "FFTComputeCommon.hlsli"

RWTexture2D<float2> Buffer0 : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint2 coordinates = id.xy;
    float2 value = Buffer0[coordinates];
    
    float divisor = Size * Size;
    float2 normalizedValue = value / divisor;
    
    Buffer0[coordinates] = normalizedValue;
}