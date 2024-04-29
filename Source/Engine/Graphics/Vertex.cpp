#pragma once
#include <DirectXMath.h>
#include <vector>
#include <windows.h>
#include <Graphics/Vertex.h>
namespace mi
{
	void VertexFunc::CalculateNormals(std::vector<BasicVertex>& vertices, const std::vector<DWORD>& indices)
	{
		// Iterate through each face (triangle) defined by the indices
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			// Indices for the three vertices of the triangle
			DWORD i1 = indices[i];
			DWORD i2 = indices[i + 1];
			DWORD i3 = indices[i + 2];

			DirectX::XMVECTOR v1 = DirectX::XMLoadFloat3(&vertices[i1].myPos);
			DirectX::XMVECTOR v2 = DirectX::XMLoadFloat3(&vertices[i2].myPos);
			DirectX::XMVECTOR v3 = DirectX::XMLoadFloat3(&vertices[i3].myPos);


			// Calculate the normal for the current triangle
			DirectX::XMVECTOR n = DirectX::XMVector3Cross(DirectX::XMVectorSubtract(v2, v1), DirectX::XMVectorSubtract(v3, v1));
			DirectX::XMFLOAT3 normal;
			DirectX::XMStoreFloat3(&normal, n);

			// Add the normal to the normals of the corresponding vertices
			vertices[i1].myNormal.x = normal.x;
			vertices[i1].myNormal.y = normal.y;
			vertices[i1].myNormal.z = normal.z;

			vertices[i2].myNormal.x = normal.x;
			vertices[i2].myNormal.y = normal.y;
			vertices[i2].myNormal.z = normal.z;

			vertices[i3].myNormal.x = normal.x;
			vertices[i3].myNormal.y = normal.y;
			vertices[i3].myNormal.z = normal.z;
		}

		// Normalize the normals
		for (size_t i = 0; i < vertices.size(); ++i)
		{
			DirectX::XMVECTOR n = DirectX::XMLoadFloat3(&vertices[i].myNormal);
			n = DirectX::XMVector3Normalize(n);
			DirectX::XMStoreFloat3(&vertices[i].myNormal, n);
		}
	}

	void VertexFunc::CalculateTangentsAndBinormals(std::vector<BasicVertex>& vertices, const std::vector<DWORD>& indices)
	{
		for (size_t i = 0; i < indices.size(); i += 3)
		{
			size_t index1 = indices[i];
			size_t index2 = indices[i + 1];
			size_t index3 = indices[i + 2];

			BasicVertex& v1 = vertices[index1];
			BasicVertex& v2 = vertices[index2];
			BasicVertex& v3 = vertices[index3];

			CalculateTangentBinormal(v1, v2, v3);
		}
	}


	void VertexFunc::CalculateTangentBinormal(BasicVertex& v1, BasicVertex& v2, BasicVertex& v3)
	{
		DirectX::XMFLOAT3 edge1 = DirectX::XMFLOAT3(v2.myPos.x - v1.myPos.x, v2.myTexCoord.y - v1.myTexCoord.y, v2.myPos.y - v1.myPos.y);
		DirectX::XMFLOAT3 edge2 = DirectX::XMFLOAT3(v3.myPos.x - v1.myPos.x, v3.myTexCoord.y - v1.myTexCoord.y, v3.myPos.y - v1.myPos.y);

		float deltaU1 = v2.myTexCoord.x - v1.myTexCoord.x;
		float deltaU2 = v3.myTexCoord.x - v1.myTexCoord.x;
		float deltaV1 = v2.myTexCoord.y - v1.myTexCoord.y;
		float deltaV2 = v3.myTexCoord.y - v1.myTexCoord.y;

		float f = 1.0f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);

		// Calculate tangent
		v1.myTangent.x = f * (deltaV2 * edge1.x - deltaV1 * edge2.x);
		v1.myTangent.y = f * (deltaV2 * edge1.y - deltaV1 * edge2.y);
		v1.myTangent.z = f * (deltaV2 * edge1.z - deltaV1 * edge2.z);
		DirectX::XMVECTOR tangent = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&v1.myTangent));

		// Accumulate the tangent to the vertex normals
		DirectX::XMVECTOR normal = DirectX::XMLoadFloat3(&v1.myNormal);
		DirectX::XMVECTOR binormal = DirectX::XMVector3Cross(normal, tangent);
		DirectX::XMVECTOR correctedBinormal = DirectX::XMVector3Cross(tangent, normal);

		DirectX::XMStoreFloat3(&v1.myTangent, tangent);
		DirectX::XMStoreFloat3(&v1.myBinormal, correctedBinormal);
	}


}