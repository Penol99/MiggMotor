#include "engine_pch.h"
#include "Texture.h"
#include <iostream>
#include <string>
#include <DDSTextureLoader.h>
#include <ErrorLog.h>

namespace mi
{
    Texture::Texture(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* textureFilePath)
        : myDevice(device), myDeviceContext(context), myShaderResourceView(nullptr)
    {
        std::wstring path(textureFilePath);
        //path = path.substr(path.find(L"Bin"), path.size()); 
        if (LoadTextureFromFile(textureFilePath))
        {
            std::wclog << L"Texture::Texture() >> Succefully loaded texture with path: " << path << std::endl;
        }
        else
        {
            LoadTextureFromFile(L"Sprites/Common/T_MissingTexture.dds");
            // C++20 TO-DO
            //std::cerr << std::wstring("Texture::Texture() >> Failed to load texture with path : " << std::wstring(textureFilePath) << std::endl;
        }
    }

    Texture::~Texture()
    {
        ReleaseResources();
    }

    bool Texture::LoadTextureFromFile(const wchar_t* filePath)
    {
        ID3D11Resource* texture = nullptr;
        HRESULT hr = DirectX::CreateDDSTextureFromFile(myDevice, filePath, &texture, &myShaderResourceView);

        if (FAILED(hr))
        {
            ErrorLog::Log(hr, "failed to load sprite texture");
            return false;
        }

        ID3D11Texture2D* pTexture = nullptr;
        hr = texture->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexture);

        if (SUCCEEDED(hr)) {
            D3D11_TEXTURE2D_DESC desc;
            pTexture->GetDesc(&desc);

            myTextureSize = { (float)desc.Width,(float)desc.Height };

            pTexture->Release();
        }
        else {
            ErrorLog::Log(hr, "failed to get texture data oopsie");
            return false;
        }

        texture->Release();

        return true;
    }

    ID3D11ShaderResourceView* Texture::GetShaderResourceView()
    {
        return myShaderResourceView;
    }
    void Texture::Bind(int slot)
    {
        if (myShaderResourceView)
        {
            myDeviceContext->PSSetShaderResources(slot, 1, &myShaderResourceView);
        }
    }

    const mi::Vector2f& Texture::GetSize() const
    {
        return myTextureSize;
    }

    void Texture::ReleaseResources()
    {
        if (myShaderResourceView)
        {
            myShaderResourceView->Release();
            myShaderResourceView = nullptr;
        }
    }
}
