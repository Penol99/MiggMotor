#pragma once
#include "Vertex.h"
#include <vector>
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "../Shaders/Shaders.h"
#include <WICTextureLoader.h>

namespace mi
{
	class Component;
	class Mesh
	{
		friend class ModelComponent;
	public:

		Mesh();
		Mesh(const Mesh& other)
		{
			myVertexBuffer = other.myVertexBuffer;
			myIndexBuffer = other.myIndexBuffer;
			myVertexShader = other.myVertexShader;
			myPixelShader = other.myPixelShader;
			myTexture = other.myTexture;
		}
		~Mesh();

		bool Init(ComPtr<ID3D11Device>& aDevice, std::vector<PBRVertex>& vertices, std::vector<DWORD>& indices, UINT numVertices, UINT numIndices);
		void Render(ID3D11DeviceContext* aDeviceContext, Component& aMaterial);
		void Render(ID3D11DeviceContext* aDeviceContext);
	private:

		VertexBuffer<PBRVertex> myVertexBuffer;
		IndexBuffer myIndexBuffer;
		VertexShader myVertexShader;
		PixelShader myPixelShader;
		ComPtr<ID3D11ShaderResourceView> myTexture;
		ComPtr<ID3D11ShaderResourceView> myReflectionTexture;
		//UINT* myStride;
	};
}

