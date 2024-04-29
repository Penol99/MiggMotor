#pragma once
#include <d3d11.h>
#include <SpriteBatch.h>
#include <vector>
#include <WICTextureLoader.h>
#include <Vector.h>

namespace mi
{
    class Texture
    {
    public:
        Texture(ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* textureFilePath);
        ~Texture();
        bool LoadTextureFromFile(const wchar_t* filePath);
        ID3D11ShaderResourceView* GetShaderResourceView();
        void Bind(int slot);
        const Vector2f& GetSize() const;

    private:

        void ReleaseResources();

        ID3D11Device* myDevice;
        ID3D11DeviceContext* myDeviceContext;
        ID3D11ShaderResourceView* myShaderResourceView;
        Vector2f myTextureSize;
        friend class Sprite;
        friend class Sprite3D;
        friend class SpriteManager;
    };
}
