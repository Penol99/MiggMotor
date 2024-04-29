#include "../PBRFunctions.hlsli"
struct SpectrumParameters
{
    float scale;
    float angle;
    float spreadBlend;
    float swell;
    float alpha;
    float peakOmega;
    float gamma;
    float shortWavesFade;
};
cbuffer CasecadeSettings : register(b10)
{

    uint Size = 512;
    float LengthScale = 100.0f;
    float CutoffHigh = 1000.0f;
    float CutoffLow = 0.001f;
 
    float GravityAcceleration = 9.81f;
    float Depth = 500.0f;
	float DeltaTime;
    float Time = 0.1f;
    
    float Lambda = 1.0f;
    bool PingPong = false;
    uint Step = 0;
    float garbagege;
};
float2 ComplexMult(float2 a, float2 b)
{
    return float2(a.r * b.r - a.g * b.g, a.r * b.g + a.g * b.r);
}

float2 ComplexExp(float2 a)
{
    return float2(cos(a.y), sin(a.y)) * exp(a.x);
}

float Frequency(float k, float g, float depth)
{
    return sqrt(g * k * tanh(min(k * depth, 20)));
}

float FrequencyDerivative(float k, float g, float depth)
{
    float th = tanh(min(k * depth, 20));
    float ch = cosh(k * depth);
    return g * (depth * k / ch / ch + th) / Frequency(k, g, depth) / 2;
}

float NormalisationFactor(float s)
{
    float s2 = s * s;
    float s3 = s2 * s;
    float s4 = s3 * s;
    if (s < 5)
        return -0.000564 * s4 + 0.00776 * s3 - 0.044 * s2 + 0.192 * s + 0.163;
    else
        return -4.80e-08 * s4 + 1.07e-05 * s3 - 9.53e-04 * s2 + 5.90e-02 * s + 3.93e-01;
}

float DonelanBannerBeta(float x)
{
    if (x < 0.95)
        return 2.61 * pow(abs(x), 1.3);
    if (x < 1.6)
        return 2.28 * pow(abs(x), -1.3);
    float p = -0.4 + 0.8393 * exp(-0.567 * log(x * x));
    return pow(10, p);
}

float DonelanBanner(float theta, float omega, float peakOmega)
{
    float beta = DonelanBannerBeta(omega / peakOmega);
    float sech = 1 / cosh(beta * theta);
    return beta / 2 / tanh(beta * 3.1416) * sech * sech;
}

float Cosine2s(float theta, float s)
{
    return NormalisationFactor(s) * pow(abs(cos(0.5 * theta)), 2 * s);
}

float SpreadPower(float omega, float peakOmega)
{
    if (omega > peakOmega)
    {
        return 9.77 * pow(abs(omega / peakOmega), -2.5);
    }
    else
    {
        return 6.97 * pow(abs(omega / peakOmega), 5);
    }
}

float DirectionSpectrum(float theta, float omega, SpectrumParameters pars)
{
    float s = SpreadPower(omega, pars.peakOmega)
		+ 16 * tanh(min(omega / pars.peakOmega, 20)) * pars.swell * pars.swell;
    return lerp(2 / 3.1415 * cos(theta) * cos(theta), Cosine2s(theta - pars.angle, s), pars.spreadBlend);
}

float TMACorrection(float omega, float g, float depth)
{
    float omegaH = omega * sqrt(depth / g);
    if (omegaH <= 1)
        return 0.5 * omegaH * omegaH;
    if (omegaH < 2)
        return 1.0 - 0.5 * (2.0 - omegaH) * (2.0 - omegaH);
    return 1;
}

float JONSWAP(float omega, float g, float depth, SpectrumParameters pars)
{
    float sigma;
    if (omega <= pars.peakOmega)
        sigma = 0.07;
    else
        sigma = 0.09;
    float r = exp(-(omega - pars.peakOmega) * (omega - pars.peakOmega)
		/ 2 / sigma / sigma / pars.peakOmega / pars.peakOmega);
	
    float oneOverOmega = 1 / omega;
    float peakOmegaOverOmega = pars.peakOmega / omega;
    return pars.scale * TMACorrection(omega, g, depth) * pars.alpha * g * g
		* oneOverOmega * oneOverOmega * oneOverOmega * oneOverOmega * oneOverOmega
		* exp(-1.25 * peakOmegaOverOmega * peakOmegaOverOmega * peakOmegaOverOmega * peakOmegaOverOmega)
		* pow(abs(pars.gamma), r);
}

float ShortWavesFade(float kLength, SpectrumParameters pars)
{
    return exp(-pars.shortWavesFade * pars.shortWavesFade * kLength * kLength);
}