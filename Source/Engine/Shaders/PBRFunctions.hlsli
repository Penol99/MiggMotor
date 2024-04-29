//// BRDF Globals
#define FLT_EPSILON 1.192092896e-07f
#define nMipOffset 3
#define MAX_REFLECTION_LOD 6
#include "Common.hlsli"



float bias(float value, float b)
{
    return (b > 0.0) ? pow(abs(value), log(b) / log(0.5)) : 0.0f;
}

float gain(float value, float g)
{
    return 0.5 * ((value < 0.5) ? bias(2.0 * value, 1.0 - g) : (2.0 - bias(2.0 - 2.0 * value, 1.0 - g)));
}

float RoughnessFromPerceptualRoughness(float perceptualRoughness)
{
    return perceptualRoughness * perceptualRoughness;
}

float PerceptualRougnessFromRoughness(float roughness)
{
    return sqrt(max(0.0, roughness));
}

float SpecularPowerFromPerceptualRoughness(float perceptualRoughness)
{
    float roughness = RoughnessFromPerceptualRoughness(perceptualRoughness);
    return (2.0 / max(FLT_EPSILON, roughness * roughness)) - 2.0;
}

float PerceptualRougnessFromSpecularPower(float specularPower)
{
    float roughness = sqrt(2.0 / (specularPower + 2.0));
    return PerceptualRougnessFromRoughness(roughness);
}

float BurleyToMip(float fPerceptualRoughness, int nMips, float NdotR)
{
    float specPower = SpecularPowerFromPerceptualRoughness(fPerceptualRoughness);
    specPower /= (4 * max(NdotR, FLT_EPSILON));
    float scale = PerceptualRougnessFromSpecularPower(specPower);
    return scale * (nMips - 1 - nMipOffset);
}

float3 GetSpecularDominantDir(float3 vN, float3 vR, float roughness)
{
    float invRough = saturate(1 - roughness);
    float alpha = invRough * (sqrt(invRough) + roughness);

    return lerp(vN, vR, alpha);
}

float GetReductionInMicrofacets(float perceptualRoughness)
{
    float roughness = RoughnessFromPerceptualRoughness(perceptualRoughness);

    return 1.0 / (roughness * roughness + 1.0);
}

float EmpiricalSpecularAO(float ao, float perceptualRoughness)
{
    float smooth = 1 - perceptualRoughness;
    float specAO = gain(ao, 0.5 + max(0.0, smooth * 0.4));

    return min(1.0, specAO + lerp(0.0, 0.5, smooth * smooth * smooth * smooth));
}

float ApproximateSpecularSelfOcclusion(float3 vR, float3 vertNormalNormalized)
{
    const float fadeParam = 1.3;
    float rimmask = clamp(1 + fadeParam * dot(vR, vertNormalNormalized), 0.0, 1.0);
    rimmask *= rimmask;

    return rimmask;
}

float3 Diffuse(float3 pAlbedo)
{
    return pAlbedo / PI;
}

float NormalDistribution_GGX(float a, float NdH)
{
    // Isotropic ggx
    float a2 = a * a;
    float NdH2 = NdH * NdH;

    float denominator = NdH2 * (a2 - 1.0f) + 1.0f;
    denominator *= denominator;
    denominator *= PI;

    return a2 / denominator;
}

float Geometric_Smith_Schlick_GGX(float a, float NdV, float NdL)
{
    // Smith Schlick-GGX
    float k = a * 0.5f;
    float GV = NdV / (NdV * (1 - k) + k);
    float GL = NdL / (NdL * (1 - k) + k);

    return GV * GL;
}

float3 Fresnel_Schlick(float3 specularColor, float3 h, float3 v)
{
    return (specularColor + (1.0f - specularColor) * pow((1.0f - saturate(dot(v, h))), 5));
}

float3 Specular(float3 specularColor, float3 h, float3 v, float a, float NdL, float NdV, float NdH)
{
    return ((NormalDistribution_GGX(a, NdH) * Geometric_Smith_Schlick_GGX(a, NdV, NdL)) * Fresnel_Schlick(specularColor, h, v)) / (4.0f * NdL * NdV + 0.0001f);
}

float SubsurfaceScatteringIntensity(float VdotN, float perceptualRoughness)
{
    float sssIntensity = 1.0 - VdotN;
    sssIntensity = pow(sssIntensity, 2.0);
    sssIntensity *= 1.0 - perceptualRoughness;
    return sssIntensity;
}

float3 SubsurfaceScatteringColor(float3 diffuseColor, float3 vN, float3 lightDir)
{
    float3 subsurfaceColor = diffuseColor;
    
    // Approximate subsurface scattering direction
    float3 scatterDir = normalize(lightDir + vN * 0.5);
    
    // Calculate subsurface scattering intensity based on the angle between the scatter direction and the view direction
    float scatterIntensity = pow(saturate(dot(scatterDir, -vN)), 2.0);
    
    // Modulate the subsurface color by the scattering intensity
    subsurfaceColor *= scatterIntensity;
    
    // Adjust the subsurface color based on the diffuse color
    subsurfaceColor *= diffuseColor;
    
    return subsurfaceColor;
}

float HorizonOcclusion(float3 vN, float3 toEye, float ao)
{
    float horizonAngle = acos(dot(vN, toEye));
    float horizonOcclusion = lerp(1.0, ao, smoothstep(0.0, 0.1, horizonAngle));
    return horizonOcclusion;
}



float3 EvaluateAmbiance(TextureCube lysBurleyCube, float3 vN, float3 VNUnit, float3 toEye, float perceptualRoughness, float ao, float3 dfcol, float3 spccol)
{
    int numMips = GetNumMips(lysBurleyCube);
    const int nrBrdMips = numMips - nMipOffset;
    float VdotN = saturate(dot(toEye, vN));//clamp(dot(toEye, vN), 0.0, 1.0f);
    const float3 vRorg = 2 * vN * VdotN - toEye;

    float3 vR = GetSpecularDominantDir(vN, vRorg, RoughnessFromPerceptualRoughness(perceptualRoughness));
    float RdotNsat = saturate(dot(vN, vR));

    float mipLevel = BurleyToMip(perceptualRoughness, numMips, RdotNsat);

    float3 specRad = lysBurleyCube.SampleLevel(defaultSampler, vR, mipLevel).xyz;
    float3 diffRad = lysBurleyCube.SampleLevel(defaultSampler, vN, (float)(nrBrdMips - 1)).xyz;

    float fT = 1.0 - RdotNsat;
    float fT5 = fT * fT;
    fT5 = fT5 * fT5 * fT;
    spccol = lerp(spccol, (float3) 1.0, fT5);

    float fFade = GetReductionInMicrofacets(perceptualRoughness);
    fFade *= EmpiricalSpecularAO(ao, perceptualRoughness);
    fFade *= ApproximateSpecularSelfOcclusion(vR, VNUnit);

    float3 ambientdiffuse = ao * dfcol * diffRad;
    float3 ambientspecular = fFade * spccol * specRad;

	return ambientdiffuse + ambientspecular;
}
float3 PlanarReflection(Texture2D renderTarget, float3 worldPos, float3 normal, float3 toEye, float3 waterColor, float3 specularColor)
{
    uint width, height;
    renderTarget.GetDimensions(width, height);
    float4 texelSize = float4(1.0 / width, 1.0 / height, width, height);
    
    // Ensure the normal is normalized
    normal = normalize(normal);
    
    float4 plane = float4(normal, -dot(normal, worldPos));
    float3 reflectionDir = reflect(-toEye, normal);

    float t = (-dot(plane, float4(worldPos, 1.0))) / dot(plane, float4(reflectionDir, 0.0));
    float3 intersectionPoint = worldPos + reflectionDir * t;

    // Check if the intersection point is within the valid range
    if (abs(intersectionPoint.x) > 1.0 || abs(intersectionPoint.z) > 1.0)
    {
        // Return the water color if the intersection point is outside the valid range
        return waterColor;
    }

    float2 reflectionTexCoord = (intersectionPoint.xz + 1.0) * 0.5;
    reflectionTexCoord.y = 1.0 - reflectionTexCoord.y;

    // Output the reflection texture coordinates as colors for debugging
    return float3(reflectionTexCoord, 0.0);
    float3 reflectedColor = renderTarget.Sample(defaultSampler, reflectionTexCoord).xyz;

    // Calculate the halfway vector
    float3 h = normalize(toEye + reflectionDir);

    // Calculate the Fresnel term using the Fresnel_Schlick function
    float3 fresnel = Fresnel_Schlick(specularColor, h, toEye);

    float3 finalColor = lerp(waterColor, reflectedColor, fresnel);

    return finalColor;
}

float3 EvaluateAmbianceAdvanced(TextureCube lysBurleyCube, float3 vN, float3 VNUnit, float3 toEye, float perceptualRoughness, float ao, float3 dfcol, float3 spccol, float3 lightDir)
{
    int numMips = GetNumMips(lysBurleyCube);
    const int nrBrdMips = numMips - nMipOffset;
    float VdotN = saturate(dot(toEye, vN));
    const float3 vRorg = 2 * vN * VdotN - toEye;

    float3 vR = GetSpecularDominantDir(vN, vRorg, RoughnessFromPerceptualRoughness(perceptualRoughness));
    float RdotNsat = saturate(dot(vN, vR));

    float mipLevel = BurleyToMip(perceptualRoughness, numMips, RdotNsat);

    float3 specRad = lysBurleyCube.SampleLevel(defaultSampler, vR, mipLevel).xyz;
    float3 diffRad = lysBurleyCube.SampleLevel(defaultSampler, vN, (float) (nrBrdMips - 1)).xyz;

    float fT = 1.0 - RdotNsat;
    float fT5 = pow(fT, 5.0);
    spccol = lerp(spccol, (float3) 1.0, fT5);

    float fFade = GetReductionInMicrofacets(perceptualRoughness);
    fFade *= EmpiricalSpecularAO(ao, perceptualRoughness);
    fFade *= ApproximateSpecularSelfOcclusion(vR, VNUnit);

    // Diffuse ambient occlusion
    float diffuseAO = lerp(1.0, ao, perceptualRoughness);

    // Improved ambient diffuse and specular terms
    float3 ambientdiffuse = diffuseAO * dfcol * diffRad;
    float3 ambientspecular = fFade * spccol * specRad;

    // Horizon occlusion

    float horizonOcclusion = HorizonOcclusion(vN, toEye, ao);
    ambientdiffuse *= horizonOcclusion;
    ambientspecular *= horizonOcclusion;

// Indirect subsurface scattering
    float3 subsurfaceColor = SubsurfaceScatteringColor(dfcol, vN, lightDir);
    float subsurfaceIntensity = SubsurfaceScatteringIntensity(VdotN, perceptualRoughness);
    float3 subsurfaceTerm = subsurfaceColor * subsurfaceIntensity * diffRad;

    return ambientdiffuse + ambientspecular + subsurfaceTerm;

}

float3 s_curve(float3 x)
{
    float a = 2.51f;
    float b = 0.03f;
    float c = 2.43f;
    float d = 0.59f;
    float e = 0.14f;
    return clamp((x * (a * x + b)) / (x * (c * x + d) + e), 0.0, 1.0);
}
float3 tonemap_s_gamut3_cine(float3 c)
{
    // based on Sony's s gamut3 cine
    float3x3 fromSrgb = float3x3(
        +0.6456794776, +0.2591145470, +0.0952059754,
        +0.0875299915, +0.7596995626, +0.1527704459,
        +0.0369574199, +0.1292809048, +0.8337616753);

    float3x3 toSrgb = float3x3(
        +1.6269474099, -0.5401385388, -0.0868088707,
        -0.1785155272, +1.4179409274, -0.2394254004,
        +0.0444361150, -0.1959199662, +1.2403560812);

    return mul(toSrgb, s_curve(mul(fromSrgb, c)));
}

float3 EvaluateDirectionalLight(float3 albedoColor, float3 specularColor, float3 normal, float roughness, float3 lightColor,float lightIntensity, float3 lightDir, float3 viewDir)
{
    // Compute som useful values
    float NdL = saturate(dot(normal, lightDir));
    float lambert = NdL; // Angle attenuation
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(lightDir + viewDir);
    float NdH = saturate(dot(normal, h));
    float VdH = saturate(dot(viewDir, h));
    float LdV = saturate(dot(lightDir, viewDir));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedoColor);
    float3 cSpec = Specular(specularColor, h, viewDir, a, NdL, NdV, NdH);

    return saturate(lightColor * lambert * lightIntensity * (cDiff * (1.0 - cSpec) + cSpec) * PI);
}

float3 EvaluatePointLight(float3 albedoColor, float3 specularColor, float3 normal, float roughness,
    float3 lightColor, float lightIntensity, float lightRange, float3 lightPos, float3 viewDir, float3 pixelPos)
{
    // Compute som useful values
    float3 lightDir = lightPos.xyz - pixelPos.xyz;
    float lightDistance = length(lightDir);
    lightDir = normalize(lightDir);
	
    float NdL = saturate(dot(normal, lightDir));
    float lambert = NdL; // Angle attenuation
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(lightDir + viewDir);
    float NdH = saturate(dot(normal, h));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedoColor);
    float3 cSpec = Specular(specularColor, h, viewDir, a, NdL, NdV, NdH);

    float linearAttenuation = lightDistance / lightRange;
    linearAttenuation = 1.0f - linearAttenuation;
    linearAttenuation = saturate(linearAttenuation);
    float physicalAttenuation = saturate(1.0f / (lightDistance * lightDistance));
    float ue4Attenuation = ((pow(saturate(1 - pow(lightDistance / lightRange, 4.0f)), 2.0f)) / (pow(lightDistance, 2.0f) + 1)); // Unreal Engine 4 attenuation
    float attenuation = lambert * linearAttenuation * physicalAttenuation;
    attenuation = ue4Attenuation * lambert;

    return saturate(lightColor * lightIntensity * attenuation * ((cDiff * (1.0 - cSpec) + cSpec) * PI));
}

float3 EvaluateSpotLight(float3 albedoColor, float3 specularColor, float3 normal,
    float roughness, float3 lightColor, float lightIntensity, float lightRange,
    float3 lightPos, float3 lightDir, float outerAngle, float innerAngle, float3 viewDir, float3 pixelPos)
{
    float3 toLight = lightPos.xyz - pixelPos.xyz;
    float lightDistance = length(toLight);
    toLight = normalize(toLight);

    float NdL = saturate(dot(normal, toLight));
    float lambert = NdL; // Angle attenuation
    float NdV = saturate(dot(normal, viewDir));
    float3 h = normalize(toLight + viewDir);
    float NdH = saturate(dot(normal, h));
    float a = max(0.001f, roughness * roughness);

    float3 cDiff = Diffuse(albedoColor);
    float3 cSpec = Specular(specularColor, h, viewDir, a, NdL, NdV, NdH);

    float cosOuterAngle = cos(outerAngle);
    float cosInnerAngle = cos(innerAngle);
    float3 lightDirection = lightDir;

    // Determine if pixel is within cone.
    float theta = dot(toLight, normalize(-lightDirection));
	// And if we're in the inner or outer radius.
    float epsilon = cosInnerAngle - cosOuterAngle;
    float intensity = clamp((theta - cosOuterAngle) / epsilon, 0.0f, 1.0f);
    intensity *= intensity;
	
    float ue4Attenuation = ((pow(saturate(1 - pow(lightDistance / lightRange, 4.0f)), 2.0f)) / (pow(lightDistance, 2.0f) + 1)); // Unreal Engine 4 attenuation
	float finalAttenuation = lambert * intensity * ue4Attenuation;

    return saturate(lightColor * lightIntensity * lambert * finalAttenuation * ((cDiff * (1.0 - cSpec) + cSpec) * PI));
}


float FresnelLerp(float f0, float f90, float VdotH)
{
    return lerp(f0, f90, pow(1.0 - VdotH, 5.0));
}

float GGXSpecular(float roughness, float NdotH, float VdotH)
{
    float alpha = roughness * roughness;
    float alphaSqr = alpha * alpha;
    float pi = 3.14159265359;
    float denom = NdotH * NdotH * (alphaSqr - 1.0) + 1.0;
    return alphaSqr / (pi * denom * denom);
}




