#include "Common.hlsli"
struct PixelInput
{
    float4 Position : SV_POSITION;
    float3 TexCoord : TEXCOORD0;
};

float4 main(PixelInput input) : SV_TARGET
{
    return environmentTexture.Sample(defaultSampler, input.TexCoord);
}