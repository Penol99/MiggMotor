#include "PBRFunctions.hlsli"

//SamplerState shadowMapSampler : register(s1);
float CalculateShadow(float4 fragPosLightSpace, float3 pixelNormal)
{
    float3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords.x = projCoords.x * 0.5 + 0.5;
    projCoords.y = projCoords.y * -0.5 + 0.5;


    float currentDepth = projCoords.z;

    float r = 1 / 2 ^ 24;
    float DepthBias = 1;
    float DepthBiasClamp = 0.0f;
    float SlopeScaledDepthBias = 1.0f;
    float Bias = (float) DepthBias * r + SlopeScaledDepthBias * -23.996;
    // Calculate texel size
    float2 texelSize = 1.0 / 6144;

    // PCF kernel size and sampling
    int kernelSize = 3; // a 3x3 kernel
    float shadow = 0.0;
    for (int x = -kernelSize / 2; x <= kernelSize / 2; ++x)
    {
        for (int y = -kernelSize / 2; y <= kernelSize / 2; ++y)
        {
            float2 samplePos = projCoords.xy + float2(x, y) * texelSize;
            float sampleDepth = shadowMap.Sample(shadowSampler, samplePos).r;
            if (currentDepth - Bias > sampleDepth)
                shadow += 1.0;
        }
    }

    shadow /= (kernelSize * kernelSize); // Average the shadow results

    return shadow;
}

PixelOutput main(ModelVertexToPixel input)
{
    PixelOutput result;

    float2 scaledUV = input.texCoord0;
	
    float3 toEye = normalize(CameraPosition.xyz - input.worldPosition.xyz);
	
    float4 albedo = albedoTexture.Sample(defaultSampler, scaledUV).rgba;

    float3 normal = normalTexture.Sample(defaultSampler, scaledUV).xyz;
	//float ambientOcclusion = 1 - normal.b;
    normal = 2.0f * normal - 1.0f;
    normal.z = sqrt(1 - saturate(normal.x * normal.x + normal.y * normal.y));
    normal = normalize(normal);

    float3x3 TBN = float3x3(
		normalize(input.tangent.xyz),
		normalize(-input.binormal.xyz),
		normalize(input.normal.xyz)
		);


    TBN = transpose(TBN);
    float3 pixelNormal = normalize(mul(TBN, normal));
	// TGA Channel Pack. MRE-S.
	// Metalness, Roughness, Emissive, Emissive Strength (opt).

    float3 material = materialTexture.Sample(defaultSampler, scaledUV).rgb;
	
    float ambientOcclusion = material.r;
    float roughness = material.g;
    float metalness = material.b;
	
    float emissive = 0.f; //1.f; 
    //material.b;

    float3 specularColor = lerp((float3) 0.04f, albedo.rgb, metalness);
    float3 diffuseColor = lerp((float3) 0.00f, albedo.rgb, 1 - metalness);

	
    float3 ambiance = EvaluateAmbiance(
		environmentTexture, pixelNormal, input.normal.xyz,
		toEye, roughness,
		ambientOcclusion, diffuseColor, specularColor
	);
	


    float3 dirLight = normalize(DirectionalLightDirection.xyz);
    float3 directionalLight = EvaluateDirectionalLight(
		diffuseColor, specularColor, pixelNormal, roughness,
		DirectionalLightColor, DirectionalLightIntensity, dirLight, toEye.xyz
	);
    float shadow = CalculateShadow(input.lightPos, pixelNormal);

	// Apply shadow to lighting
    directionalLight *= (1-shadow);

    float3 pointLights = 0;
    for (unsigned int p = 0; p < NumberOfLights; p++)
    {
        pointLights += EvaluatePointLight(
			diffuseColor, specularColor, pixelNormal, roughness,
			PointLights[p].Color.rgb, PointLights[p].Color.w, PointLights[p].Range, PointLights[p].Position.xyz,
			toEye.xyz, input.worldPosition.xyz);
    }
    float3 spotLights = 0;
    for (unsigned int s = 0; s < NumberOfSpotLights; s++)
    {
        spotLights += EvaluateSpotLight(
			diffuseColor, specularColor, pixelNormal, roughness,
			SpotLights[s].Color.rgb, SpotLights[s].Color.w, SpotLights[s].Range, SpotLights[s].Position.xyz,
			normalize(SpotLights[s].Direction.xyz), SpotLights[s].OuterAngle, SpotLights[s].InnerAngle,
			toEye.xyz, input.worldPosition.xyz);

    }
    float3 emissiveAlbedo = albedo.rgb * emissive;
    float3 radiance = ambiance + directionalLight + pointLights + spotLights + emissiveAlbedo;

    result.color.rgb = tonemap_s_gamut3_cine(radiance);
    result.color.a = 1.0f;
    return result;
}

