#include "Common.hlsli"


float3x3 invertMatrix(float3x3 m)
{
	// computes the inverse of a matrix m
    float det = m[0][0] * (m[1][1] * m[2][2] - m[2][1] * m[1][2]) -
		m[0][1] * (m[1][0] * m[2][2] - m[1][2] * m[2][0]) +
		m[0][2] * (m[1][0] * m[2][1] - m[1][1] * m[2][0]);

    float invdet = 1 / det;

    float3x3 minv; // inverse of matrix m
    minv[0][0] = (m[1][1] * m[2][2] - m[2][1] * m[1][2]) * invdet;
    minv[0][1] = (m[0][2] * m[2][1] - m[0][1] * m[2][2]) * invdet;
    minv[0][2] = (m[0][1] * m[1][2] - m[0][2] * m[1][1]) * invdet;
    minv[1][0] = (m[1][2] * m[2][0] - m[1][0] * m[2][2]) * invdet;
    minv[1][1] = (m[0][0] * m[2][2] - m[0][2] * m[2][0]) * invdet;
    minv[1][2] = (m[1][0] * m[0][2] - m[0][0] * m[1][2]) * invdet;
    minv[2][0] = (m[1][0] * m[2][1] - m[2][0] * m[1][1]) * invdet;
    minv[2][1] = (m[2][0] * m[0][1] - m[0][0] * m[2][1]) * invdet;
    minv[2][2] = (m[0][0] * m[1][1] - m[1][0] * m[0][1]) * invdet;

    return minv;
}

ModelVertexToPixel main(ModelVertexInput input) 
{

    ModelVertexToPixel result;
    
    
    float4 vertexObjectPos = input.position;
    float4 skinnedPos;
    float4x4 skinnedMatrix = 0;
    uint iBone = 0;
    float fWeight = 0;

	// Bone 0
    iBone = input.boneIndices.x;
    fWeight = input.weights.x;
    skinnedMatrix += fWeight * Bones[iBone];

	// Bone 1
    iBone = input.boneIndices.y;
    fWeight = input.weights.y;
    skinnedMatrix += fWeight * Bones[iBone];

	// Bone 2
    iBone = input.boneIndices.z;
    fWeight = input.weights.z;
    skinnedMatrix += fWeight * Bones[iBone];

	// Bone 35
    iBone = input.boneIndices.w;
    fWeight = input.weights.w;
    skinnedMatrix += fWeight * Bones[iBone];
    
    result.vertexColor0 = input.vertexColor0;
    float4x4 identityMatrix = float4x4(1.0, 0.0, 0.0, 0.0,
                                   0.0, 1.0, 0.0, 0.0,
                                   0.0, 0.0, 1.0, 0.0,
                                   0.0, 0.0, 0.0, 1.0);
    
    
    float4 vertexBoneSpace = mul(transpose(skinnedMatrix), vertexObjectPos);

    float4 vertexWorldPos = mul(ObjectWorldMatrix, vertexBoneSpace);

    float3x3 skinnedRotation = (float3x3) skinnedMatrix;
    float3x3 toWorldRotation = (float3x3) ObjectWorldMatrix;

    float3 vertexWorldBinormal = mul(toWorldRotation, mul(skinnedRotation, input.binormal));
    float3 vertexWorldTangent = mul(toWorldRotation, mul(skinnedRotation, input.tangent));
    float3 vertexWorldNormal = mul(transpose(invertMatrix(toWorldRotation)), mul(transpose(invertMatrix(skinnedRotation)), input.normal));

    result.position = mul(ProjectionMatrix, mul(ViewMatrix, vertexWorldPos));
    result.worldPosition = vertexWorldPos;
	
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
    
    //result.lightPos.w = 1;
    result.lightPos = mul(dirProjectionMatrix, mul(dirLightViewMatrix, vertexWorldPos));
	
    return result;
}