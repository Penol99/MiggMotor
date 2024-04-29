#include "Common.hlsli"

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD0; // Texture coordinates
};

float4 main(PS_INPUT input) : SV_Target
{
    return albedoTexture.Sample(defaultSampler, input.inTexCoord.xy); //the red color
}