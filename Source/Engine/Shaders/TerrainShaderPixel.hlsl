#define NUMBER_OF_LIGHTS_ALLOWED 4
#include "PBRFunctions.hlsli"

struct PS_INPUT
{
    float4 inPosition : SV_POSITION;
    float3 inNormal : NORMAL0;
    float2 inTexCoord : TEXCOORD0;
    float3 inTangent : TANGENT0;
    float3 inBinormal : BINORMAL0;
    float4 inWorldPosition : POSITION;
    float3 padding : TANGENT1;
};

Texture2D grassAlbedoTexture : register(t4);
Texture2D grassNormalTexture : register(t5);
Texture2D grassMetalTexture : register(t6);

Texture2D rockAlbedoTexture : register(t7);
Texture2D rockNormalTexture : register(t8);
Texture2D rockMetalTexture : register(t9);

Texture2D snowAlbedoTexture : register(t10);
Texture2D snowNormalTexture : register(t11);
Texture2D snowMetalTexture : register(t12);

// Hash function to generate pseudo-random noise
float hash(float2 p)
{
    p = frac(p * 0.3183099 + 0.1);
    p *= 17.0;
    return frac(p.x * p.y * (p.x + p.y));
}

// Interpolation function for smoother noise
float smoothNoise(float2 uv)
{
    float2 f = frac(uv);
    float2 u = f * f * (3.0 - 2.0 * f);
    float a = hash(floor(uv));
    float b = hash(floor(uv) + float2(1.0, 0.0));
    float c = hash(floor(uv) + float2(0.0, 1.0));
    float d = hash(floor(uv) + float2(1.0, 1.0));
    return lerp(a, b, u.x) + (c - a) * u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
}

float4 main(PS_INPUT input) : SV_TARGET
{
    float3 toEye = normalize(CameraPosition.xyz - input.inWorldPosition.xyz);
    float distance = length(CameraPosition.xyz - input.inWorldPosition.xyz);

    float uvScale = 4.0f; // Adjust this value to control the texture tiling
    float2 tiledUV = input.inTexCoord * uvScale;
    tiledUV -= floor(tiledUV);

    float height = input.inWorldPosition.y / 50;
    float peakThreshold = 0.8f;
    float middleThreshold = 0.6f; // Adjusted to increase the blend range

    // Generate procedural noise based on world position
    float noiseScale = 0.1f;
    float noise = smoothNoise(input.inWorldPosition.xz * noiseScale);
    height += noise * 0.1f; // Adjust the noise strength as needed

    float mipLevel = floor(distance / 40.0f); // Adjust the distance threshold as needed

    float3 albedoColor, normal, material;
    // Sample textures
    float3 grassAlbedo = grassAlbedoTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).rgb;
    float3 rockAlbedo = rockAlbedoTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).rgb;
    float3 snowAlbedo = snowAlbedoTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).rgb;

    float3 grassNormal = grassNormalTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).xyz;
    float3 rockNormal = rockNormalTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).xyz;
    float3 snowNormal = snowNormalTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).xyz;

    float3 grassMaterial = grassMetalTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).xyz;
    float3 rockMaterial = rockMetalTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).xyz;
    float3 snowMaterial = snowMetalTexture.SampleLevel(defaultSampler, tiledUV, mipLevel).xyz;

    // Calculate blending factors
    float grassToRock = smoothstep(0, middleThreshold, height);
    float rockToSnow = smoothstep(middleThreshold, peakThreshold, height);
    float grassToSnow = smoothstep(0, peakThreshold, height);

    // Blend textures
    albedoColor = lerp(grassAlbedo, lerp(rockAlbedo, snowAlbedo, rockToSnow), grassToRock);
    normal = lerp(grassNormal, lerp(rockNormal, snowNormal, rockToSnow), grassToRock);
    material = lerp(grassMaterial, lerp(rockMaterial, snowMaterial, rockToSnow), grassToRock);

    float ambientOcclusion = normal.b;

    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x + normal.y * normal.y));
    normal = normalize(normal);

    float3x3 TBN = float3x3(
    normalize(input.inTangent.xyz),
    normalize(-input.inBinormal.xyz),
    normalize(input.inNormal.xyz)
    );

    TBN = transpose(TBN);
    float3 pixelNormal = normalize(mul(TBN, normal));

    float metalness = material.r;
    float roughness = material.g;
    float emissive = material.b;

    float3 specularColor = lerp((float3) 0.04f, albedoColor.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedoColor.rgb, 1 - metalness);

    float3 ambiance = EvaluateAmbiance(
    environmentTexture, pixelNormal, input.inNormal.xyz,
    toEye, roughness,
    ambientOcclusion, diffuseColor, specularColor
    );

    float3 dirLight = normalize(DirectionalLightDirection.xyz);

    float3 directionalLight = EvaluateDirectionalLight(
    diffuseColor, specularColor, pixelNormal, roughness,
    DirectionalLightColor, DirectionalLightIntensity, dirLight, toEye.xyz
    );

    float3 emissiveAlbedo = albedoColor.rgb * emissive;
    float3 finalColor = ambiance + directionalLight + emissiveAlbedo;

    float minHeight = 1.0f; 

    if (input.inWorldPosition.y < minHeight)
    {
        discard;
    }
    
    
    return float4(tonemap_s_gamut3_cine(finalColor), 1);

}