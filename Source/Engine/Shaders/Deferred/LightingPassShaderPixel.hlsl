#include "../PBRFunctions.hlsli"

float CalculateShadow(float4 posLightSpace, float3 pixelNormal)
{
    
    // Transform the normal into light space
    float3 normalInLightSpace = mul(float4(pixelNormal, 1), dirProjectionMatrix).xyz;

    float3 projCoords = posLightSpace.xyz / posLightSpace.w;
    projCoords.x = projCoords.x * 0.5 + 0.5;
    projCoords.y = projCoords.y * -0.5 + 0.5;
    float currentDepth = projCoords.z;

    // Adjust bias based on the slope
    const float minBias = 0.005;
    const float maxBias = shadowBias; //0.05;
    float slope = abs(dot(normalInLightSpace, float3(0.0, 0.0, 1.0)));
    float depthBias = lerp(maxBias, minBias, slope);

    const float2 shadowMapResolution = float2(6144, 6144);
    float2 texelSize = 1.0 / shadowMapResolution;

    // PCF kernel size and sampling
    int kernelSize = 6;
    float shadow = 0.0;
    for (int x = -kernelSize / 2; x <= kernelSize / 2; ++x)
    {
        for (int y = -kernelSize / 2; y <= kernelSize / 2; ++y)
        {
            float2 samplePos = projCoords.xy + float2(x, y) * texelSize;
            samplePos = clamp(samplePos, 0.0, 1.0);
            float sampleDepth = shadowMap.Sample(shadowSampler, samplePos).r;
            if (currentDepth - depthBias > sampleDepth)
                shadow += 1.0;
        }
    }

    shadow /= (kernelSize * kernelSize);

    return shadow;
}


struct PS_INPUT
{
    float4 inPos : SV_POSITION;
    float2 inTexCoord : TEXCOORD0; // Texture coordinates
};

float4 main(PS_INPUT input) : SV_Target
{
    float2 uv = input.inTexCoord;
    //float4 diffuse = RT_Diffuse.Sample(defaultSampler, uv);
    
    //uv = diffuse.xy; 
  
    

    float4 worldPosition = RT_PositionTexture.Sample(defaultSampler, uv);
    float4 pixelNormal = RT_NormalTexture.Sample(defaultSampler, uv);
    float4 albedo = RT_AlbedoTexture.Sample(defaultSampler, uv);
    float4 inputNormal = RT_InputNormalTexture.Sample(defaultSampler, uv);
    float4 material = RT_MaterialTexture.Sample(defaultSampler, uv);
    float4 dirLightPos = RT_DirectionalLightPositionTexture.Sample(defaultSampler, uv);

    float3 toEye = normalize(CameraPosition.xyz - worldPosition.xyz);

    float shadow = CalculateShadow(dirLightPos, pixelNormal.xyz);
    shadow = 0;

    float ambientOcclusion = material.r;
    float roughness = material.g;
    float metalness = material.b;
    float emissive = 0.f; //1.f; 
    
    ambientOcclusion *= (1 - shadow);
    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness) * (1 - shadow);;
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);
    
    float3 dirLight = normalize(DirectionalLightDirection.xyz);
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, pixelNormal.xyz, roughness,
		DirectionalLightColor, DirectionalLightIntensity, dirLight, toEye.xyz
	) * (1 - shadow);

    directionalLight *= (1 - shadow);

    float3 ambiance = EvaluateAmbiance(
		environmentTexture, pixelNormal.xyz, inputNormal.xyz,
		toEye, roughness,
		ambientOcclusion, diffuseColor, specularColor
	) * (1 - shadow);

    
    float3 pointLights = 0;
    for (unsigned int p = 0; p < NumberOfLights; p++)
    {
        // Check if within light volume
        float distanceToLight = length(PointLights[p].Position.xyz - worldPosition.xyz);
        if (distanceToLight <= PointLights[p].Range)
        {
            pointLights += EvaluatePointLight(
                diffuseColor, specularColor, pixelNormal.xyz, roughness,
                PointLights[p].Color.rgb, PointLights[p].Color.w, PointLights[p].Range, PointLights[p].Position.xyz,
                toEye.xyz, worldPosition.xyz);
        }
    }
    float3 spotLights = 0;
    for (unsigned int s = 0; s < NumberOfSpotLights; s++)
    {
        // Check if within light volume
        float distanceToLight = length(SpotLights[s].Position.xyz - worldPosition.xyz);
        float angleToLight = dot(normalize(SpotLights[s].Position.xyz - worldPosition.xyz), normalize(SpotLights[s].Direction.xyz));
        if (distanceToLight <= SpotLights[s].Range && angleToLight >= cos(SpotLights[s].OuterAngle * 0.5f))
        {
            spotLights += EvaluateSpotLight(
                diffuseColor, specularColor, pixelNormal.xyz, roughness,
                SpotLights[s].Color.rgb, SpotLights[s].Color.w, SpotLights[s].Range, SpotLights[s].Position.xyz,
                normalize(SpotLights[s].Direction.xyz), SpotLights[s].OuterAngle, SpotLights[s].InnerAngle,
                toEye.xyz, worldPosition.xyz);
        }
    }

    float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 radiance = ambiance.xyz + directionalLight + pointLights + spotLights + emissiveAlbedo;

    float4 result = float4(tonemap_s_gamut3_cine(radiance), 1);
    return result;
}

