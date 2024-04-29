#include "Common.hlsli"
struct VS_INPUT
{
    float4 inPos : POSITION;
    float4 inColor : COLOR;
    float2 inTexCoord : TEXCOORD;
};
struct VS_INSTANCE
{
    float4 InstancePosition : INSTANCEPOS;
    float4 InstanceColor : INSTANCECOLOR;
    float4x4 InstanceMatrix : INSTANCEMATRIX0;
   
};
struct VS_OUTPUT
{
    float4 outPosition : SV_POSITION;
    float4 outColor : COLOR;
    float2 outTexCoord : TEXCOORD;
};
float Random(in float2 uv)
{
    float value = sin(dot(uv, float2(12.9898, 78.233))) * 43758.5453;
    return frac(value);
}

VS_OUTPUT main(VS_INPUT input, VS_INSTANCE instance)
{
    VS_OUTPUT output;

    float4 vertexObjectPos = float4(input.inPos.xyz, 1);
    float2 uv = float2(sin(time), cos(time)); // Use time or another changing value
    float randomValue = Random(uv);
  

    // Apply the instance's world matrix transformation
    vertexObjectPos.xyz = mul(float4(vertexObjectPos.xyz, 1.0f), instance.InstanceMatrix);
    //vertexObjectPos = mul(instance.InstanceMatrix, vertexObjectPos);

    vertexObjectPos.xyz += instance.InstancePosition;

    output.outColor = instance.InstanceColor;
    
    output.outPosition = mul(ProjectionMatrix, mul(ViewMatrix, vertexObjectPos));


    output.outTexCoord = input.inTexCoord;

    return output;
}

