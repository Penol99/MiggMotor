#include "FFTComputeCommon.hlsli"

Texture2D<float4> PrecomputedData : register(t53);
SamplerState PrecomputedDataSampler : register(s2);
RWTexture2D<float2> Buffer0 : register(u0);
RWTexture2D<float2> Buffer1 : register(u1);

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint2 coordinates = id.xy;
    uint2 precomputedDataCoordinates = uint2(Step, coordinates.x);
    float4 precomputedData = PrecomputedData[precomputedDataCoordinates];

    uint2 inputIndices = (uint2) precomputedData.ba;
    uint2 inputCoordinates1 = uint2(inputIndices.x, coordinates.y);
    uint2 inputCoordinates2 = uint2(inputIndices.y, coordinates.y);

    float2 complexMultiplier = float2(precomputedData.r, -precomputedData.g);

    if (PingPong)
    {
        float2 input1 = Buffer0[inputCoordinates1];
        float2 input2 = Buffer0[inputCoordinates2];
        float2 complexProduct = ComplexMult(complexMultiplier, input2);
        float2 result = input1 + complexProduct;
        Buffer1[coordinates] = result;
    }
    else
    {
        float2 input1 = Buffer1[inputCoordinates1];
        float2 input2 = Buffer1[inputCoordinates2];
        float2 complexProduct = ComplexMult(complexMultiplier, input2);
        float2 result = input1 + complexProduct;
        Buffer0[coordinates] = result;
    }
}