#pragma once
#include <DirectXMath.h>
#include <vector>
#include <windows.h>
namespace mi
{

	struct Vertex
	{
		Vertex() = default;
		Vertex(float x, float y, float z, float nx, float ny, float nz, float u, float v)
			: myPos(x, y, z),
			myNormal(nx, ny, nz),
			myTexCoord(u, v)
		{
		}
		DirectX::XMFLOAT3 myPos;
		DirectX::XMFLOAT3 myNormal;
		DirectX::XMFLOAT2 myTexCoord;
	};

	struct RenderTargetVertex
	{
		float x, y;
	};


	struct BasicVertex
	{
		BasicVertex() = default;
		BasicVertex(float x, float y, float z, float nx, float ny, float nz, float u, float v, float tx, float ty, float tz, float bx, float by, float bz)
			: myPos(x, y, z),
			myNormal(nx, ny, nz),
			myTexCoord(u, v),
			myTangent(tx, ty, tz),
			myBinormal(bx, by, bz),
			padding0(0),
			padding1(0)
		{
		}
		DirectX::XMFLOAT3 myPos;
		DirectX::XMFLOAT3 myNormal;
		DirectX::XMFLOAT2 myTexCoord;
		DirectX::XMFLOAT3 myTangent;
		DirectX::XMFLOAT3 myBinormal;
		float padding0;
		float padding1;
	};

	struct PBRVertex
	{

		DirectX::XMFLOAT4 Position = DirectX::XMFLOAT4(0, 0, 0, 0);
		DirectX::XMFLOAT4 VertexColors[4]
		{
			DirectX::XMFLOAT4(0, 0, 0, 0),
			DirectX::XMFLOAT4(0, 0, 0, 0),
			DirectX::XMFLOAT4(0, 0, 0, 0),
			DirectX::XMFLOAT4(0, 0, 0, 0),
		};

		DirectX::XMFLOAT2 UVs[4]
		{
			DirectX::XMFLOAT2(0, 0),
			DirectX::XMFLOAT2(0, 0),
			DirectX::XMFLOAT2(0, 0),
			DirectX::XMFLOAT2(0, 0)
		};

		DirectX::XMFLOAT3 Normal = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT3 Tangent = DirectX::XMFLOAT3(0, 0, 0);
		DirectX::XMFLOAT3 Binormal = DirectX::XMFLOAT3(0, 0, 0);
		float BonesIndices[4] = { -1,-1,-1,-1 };
		float BoneWeights[4] = { 0,0,0,0 };

		PBRVertex() = default;


		PBRVertex(float X, float Y, float Z, float nX, float nY, float nZ, float tX, float tY, float tZ, float bX, float bY, float bZ, float R, float G, float B, float A, float U, float V)
		{
			Position = DirectX::XMFLOAT4(X, Y, Z, 1);
			Normal = DirectX::XMFLOAT3(nX, nY, nZ);
			Tangent = DirectX::XMFLOAT3(tX, tY, tZ);
			Binormal = DirectX::XMFLOAT3(bX, bY, bZ);
			VertexColors[0] = DirectX::XMFLOAT4(R, G, B, A);
			UVs[0] = DirectX::XMFLOAT2(U, V);
		}
	};



	class VertexFunc
	{
	public:
		static void CalculateNormals(std::vector<BasicVertex>& vertices, const std::vector<DWORD>& indices);

		static void CalculateTangentsAndBinormals(std::vector<BasicVertex>& vertices, const std::vector<DWORD>& indices);
	private:
		static void CalculateTangentBinormal(BasicVertex& v1, BasicVertex& v2, BasicVertex& v3);
	};

}