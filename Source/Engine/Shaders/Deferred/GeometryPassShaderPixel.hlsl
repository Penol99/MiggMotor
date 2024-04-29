#include "../PBRFunctions.hlsli"
struct GeometryPassOutput
{
    float4 position : SV_Target0;
    float4 normal : SV_Target1;
    float4 albedo : SV_Target2;
    float4 inputNormal : SV_Target3;
    float4 material : SV_Target4;
    float4 directionalLightPos : SV_Target5;
};




GeometryPassOutput main(ModelVertexToPixel input)
{
    GeometryPassOutput output;
    float3 toEye = normalize(CameraPosition.xyz - input.worldPosition.xyz);

    
    float2 scaledUV = input.texCoord0;
    float3 normal = normalTexture.Sample(defaultSampler, scaledUV).xyz;
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

    float4 albedo = albedoTexture.Sample(defaultSampler, scaledUV).rgba;
    float3 material = materialTexture.Sample(defaultSampler, scaledUV).rgb;
    
    

    output.position = float4(input.worldPosition.xyzw);
    output.normal = float4(pixelNormal,1);
    output.albedo = float4(albedo.rgb,1);
    output.inputNormal = float4(input.normal.xyz, 1);
    output.material = float4(material.rgb, 1);
    output.directionalLightPos = float4(input.lightPos);

    return output;
}