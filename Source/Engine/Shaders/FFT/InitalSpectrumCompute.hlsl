#include "FFTComputeCommon.hlsli"

RWTexture2D<float4> WavesData : register(u0);
RWTexture2D<float2> H0K : register(u1);

Texture2D<float2> Noise : register(t53);

StructuredBuffer<SpectrumParameters> Spectrums : register(t54);

[numthreads(8, 8, 1)]
void main(uint3 id : SV_DispatchThreadID)
{
    uint2 coordinates = id.xy;
    int2 n = coordinates - Size / 2;
    float2 k = n * (2 * PI / LengthScale);
    float kLength = length(k);

    if (kLength > CutoffHigh || kLength < CutoffLow)
    {
        H0K[coordinates] = 0;
        WavesData[coordinates] = float4(k.x, 1, k.y, 0);
        return;
    }

    float kAngle = atan2(k.y, k.x);
    float omega = Frequency(kLength, GravityAcceleration, Depth);
    float dOmegadk = FrequencyDerivative(kLength, GravityAcceleration, Depth);

    float spectrum = 0;
    for (int i = 0; i < 2; i++)
    {
        if (i == 1 && Spectrums[1].scale <= 0)
            break;

        SpectrumParameters params = Spectrums[i];
        float jonswap = JONSWAP(omega, GravityAcceleration, Depth, params);
        float directionSpectrum = DirectionSpectrum(kAngle, omega, params);
        float shortWavesFade = ShortWavesFade(kLength, params);
        spectrum += jonswap * directionSpectrum * shortWavesFade;
    }

    float2 noise = Noise[coordinates];
    float2 h0k = noise * sqrt(2 * spectrum * abs(dOmegadk) / kLength * (2 * PI / LengthScale) * (2 * PI / LengthScale));

    WavesData[coordinates] = float4(k.x, 1 / kLength, k.y, omega);
    H0K[coordinates] = h0k;
}