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
    float4 outPosition : SV_POSITION;
    float3 outNormal : NORMAL0;
    float2 outTexCoord : TEXCOORD0;
    float3 outTangent : TANGENT0;
    float3 outBinormal : BINORMAL0;
    float4 outWorldPosition : POSITION;
    float3 padding : TANGENT1;
};



VS_OUTPUT main(VS_INPUT input)
{
    VS_OUTPUT output;
    
    // Create a position vector from the input position
    float4 vertexObjectPos = float4(input.position, 1);
	
    float4 vertexWorldPos = mul(ObjectWorldMatrix, vertexObjectPos);

    float3x3 toWorldRotation = (float3x3) ObjectWorldMatrix;
    float3 vertexWorldNormal = mul(toWorldRotation, input.normal);
    float3 vertexWorldBinormal = mul(toWorldRotation, input.binormal);
    float3 vertexWorldTangent = mul(toWorldRotation, input.tangent);

    output.outTexCoord = input.texCoord;
    output.outPosition = mul(ProjectionMatrix, mul(ViewMatrix, vertexWorldPos));
    output.outWorldPosition = vertexWorldPos;
	
    output.outNormal = vertexWorldNormal;
    output.outBinormal = vertexWorldBinormal;
    output.outTangent = vertexWorldTangent;
    
    return output;
}