cbuffer BlurBuffer : register(b0)
{
    float2 RenderResolution;
    bool isHorizontal;

};

Texture2D image : register(t0);
SamplerState aSampler : register(s0);

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD0;
};

float4 main(PS_INPUT input) : SV_Target
{
    float kernelSize = 6;
    float3 result = 0;
    float weightSum = 0;
    
    int kernelRadius = (int) kernelSize / 2;
    
    for (int i = -kernelRadius; i <= kernelRadius; ++i)
    {
        float weight = exp(-0.5 * (i * i) / (kernelRadius * kernelRadius));
        weightSum += weight;
        
        float2 offset;
        if (isHorizontal)
        {
            offset = float2(i, 0.0) / RenderResolution.x;
        }
        else
        {
            offset = float2(0.0, i) / RenderResolution.y;
        }
        
        result += image.Sample(aSampler, input.inTexCoord.xy + offset).rgb * weight;
    }
    
    result /= weightSum;
    
    return float4(result, 1.0);
}




//Texture2D aTexture : register(t0); // Texture to sample
//SamplerState aSampler : register(s0); // Sampler for texture sampling

//struct PS_INPUT
//{
//    float4 inPos : SV_POSITION;
//    float2 inTexCoord : TEXCOORD0; // Texture coordinates
//};


//float4 main(PS_INPUT input) : SV_Target
//{
//    
//    float4 color = aTexture.Sample(aSampler, input.inTexCoord);

//   
//    float blurAmount = 0.02f; // Adjust the blur amount as needed

//    float4 blurredColor = 0;
//    int numSamples = 12; // Number of samples in the blur

//    // Sample and accumulate colors for the blur
//    for (int i = -1; i <= 1; ++i)
//    {
//        for (int j = -1; j <= 1; ++j)
//        {
//            float2 offset = float2(i, j) * blurAmount;
//            blurredColor += aTexture.Sample(aSampler, input.inTexCoord + offset);
//        }
//    }

//    // Average the accumulated colors
//    blurredColor = color * float4(0, 0, 1,1); // /= numSamples;

//    return blurredColor;