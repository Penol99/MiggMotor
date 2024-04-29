#pragma once
#include <d3d11.h>
#include <wrl/client.h>
using namespace Microsoft::WRL;

namespace mi
{

	class IndexBuffer
	{
	private:
		IndexBuffer(const IndexBuffer& rhs);
		ComPtr<ID3D11Buffer> myBuffer;
		UINT myBufferSize = 0;
	public:
		IndexBuffer() {}

		ID3D11Buffer* Get()const
		{
			return myBuffer.Get();
		}
		ID3D11Buffer* const* GetAddressOf()const
		{
			return myBuffer.GetAddressOf();
		}

		UINT GetBufferSize()const
		{
			return myBufferSize;
		}

		HRESULT Init(ID3D11Device* aDevice, DWORD* someData, UINT numIndicies)
		{
			if (myBuffer.Get() != nullptr)
			{
				myBuffer.Reset();
			}
			myBufferSize = numIndicies;

			D3D11_BUFFER_DESC indexBufferDesc;
			ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));

			indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
			indexBufferDesc.ByteWidth = sizeof(DWORD) * numIndicies;
			indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			indexBufferDesc.CPUAccessFlags = 0;
			indexBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA indexBufferData;
			ZeroMemory(&indexBufferData, sizeof(indexBufferData));

			indexBufferData.pSysMem = someData;

			return aDevice->CreateBuffer(&indexBufferDesc, &indexBufferData, myBuffer.GetAddressOf());

		}

	};
}
