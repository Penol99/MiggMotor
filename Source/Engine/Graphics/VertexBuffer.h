#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

using namespace Microsoft::WRL;

namespace mi
{
	template<class T>
	class VertexBuffer
	{
	private:
		VertexBuffer(const VertexBuffer<T>& rhs);
		ComPtr<ID3D11Buffer> mBuffer;
		UINT mStride = sizeof(T);
		UINT mBufferSize = 0;
	public:
		VertexBuffer() {}

		ID3D11Buffer* Get()const
		{
			return mBuffer.Get();
		}
		ID3D11Buffer* const* GetAddressOf()const
		{
			return mBuffer.GetAddressOf();
		}

		UINT GetBufferSize()const
		{
			return mBufferSize;
		}
		const UINT GetStride() const
		{
			return mStride;
		}
		const UINT* GetStridePtr() const
		{
			return &mStride;
		}
		HRESULT Init(ID3D11Device* aDevice, T* someData, UINT numVertices)
		{
			if (mBuffer.Get() != nullptr)
			{
				mBuffer.Reset();
			}
			mBufferSize = numVertices;


			D3D11_BUFFER_DESC vertexBufferDesc;
			ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

			vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			vertexBufferDesc.ByteWidth = sizeof(T) * numVertices;
			vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			vertexBufferDesc.CPUAccessFlags = 0;
			vertexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA vertexBufferData;
			ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));

			vertexBufferData.pSysMem = someData;

			return aDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, mBuffer.GetAddressOf());

		}

	};
}

