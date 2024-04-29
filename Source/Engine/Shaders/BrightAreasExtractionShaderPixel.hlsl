Texture2D aTexture : register(t0); 
SamplerState aSampler : register(s0); 

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD0; // Texture coordinates
};

float4 main(PS_INPUT input) : SV_Target
{
    float4 color = aTexture.Sample(aSampler, input.inTexCoord);

    // Convert to grayscale (luminance)
    float luminance = dot(color.rgb, float3(0.3, 0.59, 0.11)); //0.2126, 0.7152, 0.0722));

    // Threshold to extract bright areas
    float threshold = 1.5; 
    color.rgb = saturate(luminance > threshold ? color.rgb : float3(0, 0, 0));

    return color;


}