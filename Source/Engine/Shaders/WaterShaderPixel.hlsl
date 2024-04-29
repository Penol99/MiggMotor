#include "FFT/FFTComputeCommon.hlsli"

Texture2D _Derivatives_c0 : register(t15);
Texture2D _Derivatives_c1 : register(t16);
Texture2D _Derivatives_c2 : register(t17);
Texture2D _Turbulence_c0 : register(t18);
Texture2D _Turbulence_c2 : register(t19);
Texture2D _Turbulence_c1 : register(t20);


float3 EvaluateAmbianceWater(TextureCube environmentTexture, float3 normal, float3 viewDir, float roughness, float ao, float3 diffuseColor, float3 specularColor)
{
    float3 reflectionDir = reflect(-viewDir, normal);
    float3 halfVector = normalize(viewDir + reflectionDir);
    float VdotH = saturate(dot(viewDir, halfVector));
    
    float mip = roughness * 1;
    float3 prefilteredColor = environmentTexture.SampleLevel(defaultSampler, reflectionDir, mip).rgb;
    
    float fresnel = FresnelLerp(max(roughness, 0.05), 1.0, VdotH);
    fresnel = lerp(fresnel, 0.0, roughness); 
    
    float3 indirectDiffuse = diffuseColor * ao;
    float3 indirectSpecular = prefilteredColor * (specularColor * fresnel);
    
    return indirectDiffuse + indirectSpecular;
}



struct PS_INPUT
{
    float4 position : SV_POSITION;
    float2 worldUV : TEXCOORD0;
    float4 lodScales : TEXCOORD1;
    float3 viewVector : TEXCOORD2;
    float3 worldNormal : TEXCOORD3;
    float3 worldTangent : TEXCOORD4;
    float3 worldBitangent : TEXCOORD5;
    float4 screenPos : TEXCOORD6;
    float4 worldPos : POSITION2;
};



float4 main(PS_INPUT input) : SV_TARGET
{
    const float epsilon = 0.01;
    float2 uv = input.worldUV.xy;
    float LengthScale0 = 250;
    float LengthScale1 = 17;
    float LengthScale2 = 5;
    float textureSize = 256;
    
    float numMipLevels = log2(max(textureSize, textureSize)) + 1;

    float maxDistance = 2000.0;
    float3 cameraToPixel = CameraPosition.xyz - input.worldPos.xyz;
    float distance = length(cameraToPixel);
    float mipLevel = log2(distance / maxDistance * (numMipLevels - 1));
    
    float4 derivatives = _Derivatives_c0.SampleLevel(waterSampler, input.worldUV / LengthScale0, mipLevel);

#if defined(MID) || defined(CLOSE)
    derivatives += _Derivatives_c1.SampleLevel(waterSampler, input.worldUV / LengthScale1, mipLevel) / input.lodScales.y;
#endif

#if defined(CLOSE)
    derivatives += _Derivatives_c2.SampleLevel(waterSampler, input.worldUV / LengthScale2, mipLevel) / input.lodScales.z;
#endif

    float2 slope = float2(derivatives.x / (1 + derivatives.z),
            derivatives.y / (1 + derivatives.w));
    float3 worldNormal = normalize(float3(-slope.x, 1, -slope.y));
    
    worldNormal = 2.0f * worldNormal - 1.0f;
    worldNormal.z = sqrt(1 - saturate(worldNormal.x * worldNormal.x + worldNormal.y * worldNormal.y));
    worldNormal = normalize(worldNormal);
    
    
    float3x3 worldToTangent = float3x3(
    float3(input.worldTangent),
    float3(-input.worldBitangent),
    float3(input.worldNormal));
    float3 pixelNormal = mul(worldToTangent, worldNormal);


    float3 displacedPosition = input.worldPos;
    
    float3 toEye = normalize(CameraPosition.xyz - displacedPosition);


    float depth = DepthViewTexture.Sample(defaultSampler, uv).r;
    float4 view = mul(ProjectionMatrix, float4(uv, depth, 1.0));
    view.xyz /= view.w;
    float linearDepth = -view.z;
    float linearDist = length(displacedPosition - CameraPosition.xyz);


    float refractionDepthFactor = 0.1;
    float refractionFactor = clamp(linearDepth * refractionDepthFactor, 0.1, 0.5);
    float3 refractedDir = refract(toEye, pixelNormal, 1.0 / 1.33);
    float3 refractedColor = environmentTexture.Sample(defaultSampler, refractedDir).rgb;

    float4x4 ViewProjectionMatrix = mul(ProjectionMatrix, ViewMatrix);

    // Planar Reflection
    float3 reflectionPlaneNormal = float3(0, 1, 0);
    float3 reflectionPlanePoint = float3(0, 0, 0);

    float3 reflectionPos = displacedPosition - 2.0 * dot(displacedPosition - reflectionPlanePoint, reflectionPlaneNormal) * reflectionPlaneNormal;

    float4 reflectionWorldPos = float4(reflectionPos, 1.0);
    float4 reflectionViewPos = mul(ViewMatrix, reflectionWorldPos);
    float4 reflectionProjPos = mul(ProjectionMatrix, reflectionViewPos);

    float3 reflectionUV = reflectionProjPos.xyz / reflectionProjPos.w;
    reflectionUV.x = reflectionUV.x * 0.5 + 0.5;
    reflectionUV.y = reflectionUV.y * 0.5 + 0.5;

    float3 reflectionColor = TerrainViewTexture.Sample(defaultSampler, reflectionUV.xy).rgb;


    float3 envReflectionDir = reflect(-toEye, pixelNormal);
    float3 envReflectionColor = environmentTexture.Sample(defaultSampler, envReflectionDir).rgb;


    float fresnelPower = 1.0;
    float fresnelFactor = pow(1.0 - dot(pixelNormal, toEye), fresnelPower);
    float fresnel = fresnelFactor;

    float3 finalReflectionColor = lerp(reflectionColor, envReflectionColor, fresnel);


    float roughness = lerp(0.02, 0.6, 0.2);
    float metalness = lerp(0.7, 0.0, 0.1);




   
    
    float3 dirLight = normalize(DirectionalLightDirection.xyz);
   // float3 directionalLight = EvaluateDirectionalLight(deepColor, specularColor, pixelNormal, roughness, DirectionalLightColor, DirectionalLightIntensity, dirLight, toEye);

    float3 finalColor = finalReflectionColor; //+directionalLight;

    return float4(finalColor, 1);

}