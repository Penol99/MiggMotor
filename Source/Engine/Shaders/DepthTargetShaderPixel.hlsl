#include "Common.hlsli"
struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD0; // Texture coordinates
};

float4 main(PS_INPUT input) : SV_Target
{
    float4 color = albedoTexture.Sample(shadowSampler, input.inTexCoord.xy);
    color.r = color.r;
    color.b = color.r;
    color.g = color.r;
    return color;
}