#include <vector>
#include <random>
#include <cmath>
#include <vector>
#include "Vertex.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "../Shaders/Shaders.h"
#include "Camera.h"
#include "BasicPerlinNoise.hpp"
#include <wrl/client.h>

using namespace Microsoft::WRL;
namespace mi
{
    class TerrainGenerator
    {
    public:
        TerrainGenerator();

        TerrainGenerator(const TerrainGenerator& other);



        bool Init(ComPtr<ID3D11Device>& aDevice,
            ComPtr<ID3D11DeviceContext>& aDeviceContext,
            Camera& aCamera,
            int aWidth,
            int aHeight,
            float aScale,
            int someOctaves,
            float aPersistence,
            float aLacunarity,
            int aSeed);



        void Render(ID3D11DeviceContext* aDeviceContext);
        DirectX::XMFLOAT3 myRotation = { 0.0f, 0.0f, 0.0f };
        DirectX::XMFLOAT3 myPosition = { 0.0f, 0.0f, 0.0f };
    private:


        DirectX::XMFLOAT3 myScale = { 1.0f,100.f,1.0f };
        Camera* myCamera = nullptr;
        void InitTextures();
        void BindTextures();

        int myWidth;
        int myHeight;
        float myPerlinScale;
        int myOctaves;
        float myPersistence;
        float myLacunarity;
        int mySeed;

        ConstantBuffer<CB_ObjectWorldMatrix> myConstantBuffer;

        VertexBuffer<BasicVertex> myVertexBuffer;
        IndexBuffer myIndexBuffer;

        VertexShader myVertexShader;
        PixelShader myPixelShader;
        siv::BasicPerlinNoise<float> myPerlinNoise;

        ComPtr<ID3D11ShaderResourceView> myGrassAlbedoTexture;
        ComPtr<ID3D11ShaderResourceView> myGrassNormalTexture;
        ComPtr<ID3D11ShaderResourceView> myGrassMetalTexture;

        ComPtr<ID3D11ShaderResourceView> myRockAlbedoTexture;
        ComPtr<ID3D11ShaderResourceView> myRockNormalTexture;
        ComPtr<ID3D11ShaderResourceView> myRockMetalTexture;

        ComPtr<ID3D11ShaderResourceView> mySnowAlbedoTexture;
        ComPtr<ID3D11ShaderResourceView> mySnowNormalTexture;
        ComPtr<ID3D11ShaderResourceView> mySnowMetalTexture;

        ComPtr<ID3D11ShaderResourceView> myCubeMap;



        std::vector<float> GenerateTerrain();

    };
}
