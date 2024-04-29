#include "Common.hlsli"

ModelVertexToPixel main(ModelVertexInput input) 
{
	ModelVertexToPixel result;


    float4 vertexObjectPos = input.position; 
	
    float4 vertexWorldPos = mul(ObjectWorldMatrix, vertexObjectPos);

    float3x3 toWorldRotation = (float3x3) ObjectWorldMatrix;
    float3 vertexWorldNormal = mul(toWorldRotation, input.normal);
    float3 vertexWorldBinormal = mul(toWorldRotation, input.binormal);
    float3 vertexWorldTangent = mul(toWorldRotation, input.tangent);

    result.position = mul(ProjectionMatrix, mul(ViewMatrix, vertexWorldPos));
    result.worldPosition = vertexWorldPos;
	
	result.vertexColor0 = input.vertexColor0;
	result.vertexColor1 = input.vertexColor1;
	result.vertexColor2 = input.vertexColor2;
	result.vertexColor3 = input.vertexColor3;

	result.texCoord0 = input.texCoord0;
	result.texCoord1 = input.texCoord1;
	result.texCoord2 = input.texCoord2;
	result.texCoord3 = input.texCoord3;

	result.normal = vertexWorldNormal;
	result.binormal = vertexWorldBinormal;
	result.tangent = vertexWorldTangent;
	
    result.lightPos = mul(dirProjectionMatrix, mul(dirLightViewMatrix, vertexWorldPos));
	
	
	return result;
}