// Combine Bloom and Scene Pixel Shader
Texture2D SceneTexture : register(t0);
Texture2D BloomTexture : register(t1);
SamplerState LinearSampler : register(s0);

struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD0; // Texture coordinates
};

float4 main(PS_INPUT input) : SV_Target
{
    const float gamma = 2.2;
    const float exposure = 1.5f;
    // Sample the scene texture and bloom texture
    float4 sceneColor = SceneTexture.Sample(LinearSampler, input.inTexCoord);
    float4 bloomColor = BloomTexture.Sample(LinearSampler, input.inTexCoord);

    // Combine the scene and bloom using additive blending
    float3 combinedColor = sceneColor.rgb + bloomColor.rgb;
    float3 result = float3(1.0,1.0,1.0) - exp(-combinedColor * exposure);

    return float4(combinedColor, 1);
}