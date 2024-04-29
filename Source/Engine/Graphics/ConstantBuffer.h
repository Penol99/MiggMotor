#pragma once

#include <d3d11.h>
#include "ConstantBufferTypes.h"
#include <wrl/client.h>
#include "ErrorLog.h"
#include <iostream>

using namespace Microsoft::WRL;

namespace mi
{

	template<class T>
	class ConstantBuffer
	{
	private:
		ConstantBuffer(const ConstantBuffer<T>& rhs);
		ComPtr<ID3D11Buffer> myBuffer;
		ID3D11DeviceContext* myDeviceContext = nullptr;
	public:
		ConstantBuffer() {}
		~ConstantBuffer()
		{
			int a = 1;
			a;
		}

		T myData;

		ID3D11Buffer* Get()const
		{
			return myBuffer.Get();
		}
		ID3D11Buffer* const* GetAddressOf()const
		{
			return myBuffer.GetAddressOf();
		}


		HRESULT Init(ID3D11Device* aDevice, ID3D11DeviceContext* aDeviceContext)
		{
			if (myBuffer.Get() != nullptr)
			{
				myBuffer.Reset();
			}


			myDeviceContext = aDeviceContext;
			D3D11_BUFFER_DESC constantBufferDesc;
			constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			constantBufferDesc.MiscFlags = 0;

			constantBufferDesc.ByteWidth = sizeof(T);
			constantBufferDesc.StructureByteStride = 0;

			return aDevice->CreateBuffer(&constantBufferDesc, 0, myBuffer.GetAddressOf());

		}

		bool ApplyChanges()
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = myDeviceContext->Map(myBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(hr))
			{
				ErrorLog::Log(hr, "failed mapping constant buffer");
				return false;
			}
			CopyMemory(mappedResource.pData, &myData, sizeof(T));
			myDeviceContext->Unmap(myBuffer.Get(), 0);
			return true;
		}

	};
}
