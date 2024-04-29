#include "FFTComputeCommon.hlsli"

RWTexture2D<float2> NoiseTexture : register(u0);

float NormalRandom(float2 uv)
{
    float r = sqrt(-2.0 * log(uv.x));
    float theta = 2.0 * PI * uv.y;
    return r * cos(theta);
}

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint width, height;
    NoiseTexture.GetDimensions(width, height);

    float2 uv = (id.xy + 0.5) / float2(width, height);


    float2 rand1 = frac(sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453);
    float2 rand2 = frac(sin(dot(uv + rand1, float2(12.9898, 78.233))) * 43758.5453);


    float noise1 = NormalRandom(rand1);
    float noise2 = NormalRandom(rand2);

    NoiseTexture[id.xy] = float2(noise1, noise2);
}