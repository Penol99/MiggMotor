#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include <memory>

using namespace Microsoft::WRL;

namespace mi
{

	template<class T>
	class InstanceBuffer
	{
	private:
		InstanceBuffer(const InstanceBuffer<T>& rhs);
		ComPtr<ID3D11Buffer> mBuffer;
		UINT mStride = sizeof(T);
		UINT mBufferSize = 0;
	public:
		InstanceBuffer() {}

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
		HRESULT Init(ID3D11Device* aDevice, T* someData, UINT numInstances)
		{

			if (mBuffer.Get() != nullptr)
			{
				mBuffer.Reset();
			}
			mBufferSize = numInstances;

			D3D11_BUFFER_DESC instanceBufferDesc;
			ZeroMemory(&instanceBufferDesc, sizeof(instanceBufferDesc));

			instanceBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
			instanceBufferDesc.ByteWidth = sizeof(T) * numInstances;
			instanceBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			instanceBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			instanceBufferDesc.MiscFlags = 0;

			D3D11_SUBRESOURCE_DATA instanceBufferData;
			ZeroMemory(&instanceBufferData, sizeof(instanceBufferData));

			instanceBufferData.pSysMem = someData;

			return aDevice->CreateBuffer(&instanceBufferDesc, &instanceBufferData, mBuffer.GetAddressOf());
		}
		HRESULT UpdateBuffer(ID3D11DeviceContext* aDeviceContext, T* someData)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			HRESULT hr = aDeviceContext->Map(mBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			if (FAILED(hr))
			{
				ErrorLog::Log(hr, "failed memcopy.");
				return hr;
			}

			// Copy your data into mappedResource.pData
			memcpy(mappedResource.pData, someData, sizeof(T) * mBufferSize);

			// Unmap the buffer
			aDeviceContext->Unmap(mBuffer.Get(), 0);

			return S_OK;
		}

	};
}
