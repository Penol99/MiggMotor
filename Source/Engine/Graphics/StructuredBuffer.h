#pragma once

#include <d3d11.h>
#include <wrl/client.h>
#include "ErrorLog.h"

using namespace Microsoft::WRL;

namespace mi
{
    template <typename T>
    class StructuredBuffer
    {
    private:
        StructuredBuffer(const StructuredBuffer<T>& rhs) = delete;

        ComPtr<ID3D11Buffer> myBuffer;
        ComPtr<ID3D11Buffer> myStagingBuffer;

        ID3D11DeviceContext* myDeviceContext = nullptr;
        UINT myElementCount = 0;

    public:
        StructuredBuffer() = default;
        ~StructuredBuffer() = default;

        HRESULT Init(ID3D11Device* device, ID3D11DeviceContext* deviceContext, UINT elementCount)
        {
            if (myBuffer.Get() != nullptr)
            {
                myBuffer.Reset();
            }

            myDeviceContext = deviceContext;
            myElementCount = elementCount;

            D3D11_BUFFER_DESC bufferDesc = {};
            bufferDesc.ByteWidth = sizeof(T) * elementCount;
            bufferDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_UNORDERED_ACCESS;
            bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
            bufferDesc.StructureByteStride = sizeof(T);

            //D3D11_SUBRESOURCE_DATA initData = {};
            //initData.pSysMem = NULL;

            HRESULT hr = device->CreateBuffer(&bufferDesc, NULL, myBuffer.GetAddressOf());
            if (FAILED(hr))
            {
                ErrorLog::Log(hr, "Failed to create structured buffer");
                return hr;
            }


            D3D11_BUFFER_DESC stagingBufferDesc = {};
            stagingBufferDesc.ByteWidth = sizeof(T) * elementCount;
            stagingBufferDesc.Usage = D3D11_USAGE_STAGING;
            stagingBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

            hr = device->CreateBuffer(&stagingBufferDesc, nullptr, myStagingBuffer.GetAddressOf());
            if (FAILED(hr))
            {
                ErrorLog::Log(hr, "Failed to create staging buffer");
                return hr;
            }

            return S_OK;
        }

        ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Device* device)
        {
            ID3D11ShaderResourceView* srv = nullptr;
            D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
            srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
            srvDesc.BufferEx.FirstElement = 0;
            srvDesc.BufferEx.NumElements = myElementCount;
            srvDesc.Format = DXGI_FORMAT_UNKNOWN;

            HRESULT hr = device->CreateShaderResourceView(myBuffer.Get(), &srvDesc, &srv);
            if (FAILED(hr))
            {
                ErrorLog::Log(hr, "Failed to create shader resource view for structured buffer");
                return nullptr;
            }

            return srv;
        }

        ID3D11UnorderedAccessView* CreateUnorderedAccessView(ID3D11Device* device)
        {
            ID3D11UnorderedAccessView* uav = nullptr;
            D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
            uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
            uavDesc.Buffer.FirstElement = 0;
            uavDesc.Buffer.NumElements = myElementCount;
            uavDesc.Format = DXGI_FORMAT_UNKNOWN;

            HRESULT hr = device->CreateUnorderedAccessView(myBuffer.Get(), &uavDesc, &uav);
            if (FAILED(hr))
            {
                ErrorLog::Log(hr, "Failed to create unordered access view for structured buffer");
                return nullptr;
            }

            return uav;
        }

        ID3D11Buffer* Get() const
        {
            return myBuffer.Get();
        }

        ID3D11Buffer* const* GetAddressOf() const
        {
            return myBuffer.GetAddressOf();
        }

        UINT GetElementCount() const
        {
            return myElementCount;
        }

        void Apply(ID3D11DeviceContext* deviceContext, const T* data, UINT elementCount)
        {
            D3D11_MAPPED_SUBRESOURCE mappedResource;
            HRESULT hr = deviceContext->Map(myStagingBuffer.Get(), 0, D3D11_MAP_WRITE, 0, &mappedResource);
            if (FAILED(hr))
            {
                ErrorLog::Log(hr, "Failed to map staging buffer for update");
                return;
            }

            memcpy(mappedResource.pData, data, sizeof(T) * elementCount);
            deviceContext->Unmap(myStagingBuffer.Get(), 0);

            deviceContext->CopyResource(myBuffer.Get(), myStagingBuffer.Get());
        }
    };

}