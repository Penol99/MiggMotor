#include "FFTComputeCommon.hlsli"

RWTexture2D<float2> H0 : register(u0);
RWTexture2D<float2> H0K : register(u1);

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint2 coordinates = id.xy;
    uint2 mirroredCoordinates = uint2((Size - coordinates.x) % Size, (Size - coordinates.y) % Size);

    float2 h0K = H0K[coordinates];
    float2 h0MinusK = H0K[mirroredCoordinates];

    float4 combinedValues = float4(h0K.x, h0K.y, h0MinusK.x, -h0MinusK.y);
    H0[coordinates] = combinedValues;
}