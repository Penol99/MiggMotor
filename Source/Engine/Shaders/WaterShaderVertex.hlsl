#include "Common.hlsli"
struct VS_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 texCoord : TEXCOORD0;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
};

struct VS_OUTPUT
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


Texture2D _Displacement_c0 : register(t54);
Texture2D _Displacement_c1 : register(t55);
Texture2D _Displacement_c2 : register(t56);

float4x4 IdentityMatrix =
{
    1,0,0,0,
    0,1,0,0,
    0,0,1,0,
    0,0,0,1
};

VS_OUTPUT main(VS_INPUT input)
{
    
    float LengthScale0 = 250;
    float LengthScale1 = 17;
    float LengthScale2 = 5;
    float _LOD_scale = 7.13;
    float _SSSBase = 0;
    float _SSSScale = 4;
    
    VS_OUTPUT output;
    
    float3 worldPos = mul(ObjectWorldMatrix, float4(input.position,1));
    output.worldPos = float4(worldPos,1);
    float4 worldUV = float4(worldPos.xz, 0, 0);
    output.worldUV = worldUV.xy;

    output.viewVector = CameraPosition.xyz - mul(ObjectWorldMatrix, float4(input.position, 1)).xyz;
    float viewDist = length(output.viewVector);
    
    float lod_c0 = min(_LOD_scale * LengthScale0 / viewDist, 1);
    float lod_c1 = min(_LOD_scale * LengthScale1 / viewDist, 1);
    float lod_c2 = min(_LOD_scale * LengthScale2 / viewDist, 1);
    
    float3 displacement = 0;
    float largeWavesBias = 0;
    
    displacement += _Displacement_c0.SampleLevel(defaultSampler, worldUV.xy / LengthScale0, lod_c0).xyz * lod_c0;
    largeWavesBias = displacement.y;
#if defined(MID) || defined(CLOSE)
    displacement += _Displacement_c1.SampleLevel(defaultSampler, worldUV.xy / LengthScale1, lod_c1).xyz * lod_c1;
#endif
#if defined(CLOSE)
    displacement += _Displacement_c2.SampleLevel(defaultSampler, worldUV.xy / LengthScale2, lod_c2).xyz * lod_c2;
#endif

    float3 displacedPos = input.position + displacement;
    
    float3 displacedNormal = normalize(input.normal + displacement);

    float3 displacedTangent = normalize(input.tangent + displacement);
    

    float3 displacedBinormal = normalize(cross(displacedNormal, displacedTangent));
    
    worldPos = mul(ObjectWorldMatrix, float4(displacedPos, 1));

    output.lodScales = float4(lod_c0, lod_c1, lod_c2, max(displacement.y - largeWavesBias * 0.8 - _SSSBase, 0) / _SSSScale);
    
    output.position = mul(ProjectionMatrix, mul(ViewMatrix, float4(worldPos, 1)));
    
    output.worldNormal = mul((float3x3) ObjectWorldMatrix, displacedNormal);
    output.worldTangent = mul((float3x3) ObjectWorldMatrix, displacedTangent);
    output.worldBitangent = mul((float3x3) ObjectWorldMatrix, displacedBinormal);
    output.screenPos = output.position;

    return output;

}
