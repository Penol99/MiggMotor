#include "Common.hlsli"
struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float4 inColor : COLOR;
    float2 inTexCoords : TEXCOORD; // Texture coordinates input
    //float age : AGE; // Age input
};
Texture2D albedoTextureaa : register(t10);



float4 main(PS_INPUT input) : SV_TARGET
{
    float4 pixelColor = albedoTextureaa.Sample(defaultSampler, input.inTexCoords.xy).rgba + float4(input.inColor.xyz,0);
  


    //pixelColor.xyz = (input.inColor.xyz);
   pixelColor.a *= (input.inColor.a);
    
    
    return pixelColor;
}

