struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float2 inTexCoord : TEXCOORD0;
};

Texture2D objTexture : TEXTURE : register(t0);
SamplerState objSamplerState : SAMPLER : register(s0);

float4 main(PS_INPUT input) : SV_TARGET
{
    float4 pixelColor = float4(objTexture.Sample(objSamplerState, input.inTexCoord.xy).rgb,1);
    return float4(0.8,0.2, 0.7,.5);
}