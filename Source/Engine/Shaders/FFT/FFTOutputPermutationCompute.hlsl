#include "FFTComputeCommon.hlsli"

RWTexture2D<float2> Buffer0 : register(u0);

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint2 coordinates = id.xy;
    bool isCheckerboardPattern = ((coordinates.x + coordinates.y) % 2) == 0;
    float signFlip = isCheckerboardPattern ? 1.0 : -1.0;

    float2 originalValue = Buffer0[coordinates];
    float2 modifiedValue = originalValue * signFlip;

    Buffer0[coordinates] = modifiedValue;
}