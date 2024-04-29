#include "TerrainGenerator.h"
#include <iostream>
#include "Engine.h"
#include <WICTextureLoader.h>
#include <DDSTextureLoader.h>
#include <Vector.h>

namespace mi
{
	TerrainGenerator::TerrainGenerator() : myWidth(0), myHeight(0), myPerlinScale(0.0f), myOctaves(0),
		myPersistence(0.0f), myLacunarity(0.0f), mySeed(0)
	{
	}

	TerrainGenerator::TerrainGenerator(const TerrainGenerator& other)
		: myWidth(other.myWidth),
		myHeight(other.myHeight),
		myPerlinScale(other.myPerlinScale),
		myOctaves(other.myOctaves),
		myPersistence(other.myPersistence),
		myLacunarity(other.myLacunarity),
		mySeed(other.mySeed),
		myCamera(other.myCamera),
		myRotation(other.myRotation),
		myPosition(other.myPosition),
		myScale(other.myScale)
	{
		myVertexShader = other.myVertexShader;
		myPixelShader = other.myPixelShader;
		myConstantBuffer = other.myConstantBuffer;
		myVertexBuffer = other.myVertexBuffer;
		myIndexBuffer = other.myIndexBuffer;
	}

	bool TerrainGenerator::Init(ComPtr<ID3D11Device>& aDevice,
		ComPtr<ID3D11DeviceContext>& aDeviceContext,
		Camera& aCamera,
		int aWidth,
		int aHeight,
		float aPerlinScale,
		int someOctaves,
		float aPersistence,
		float aLacunarity,
		int aSeed)
	{
		myWidth = aWidth;
		myHeight = aHeight;
		myPerlinScale = aPerlinScale;
		myOctaves = someOctaves;
		myPersistence = aPersistence;
		myLacunarity = aLacunarity;
		mySeed = aSeed;

		myCamera = &aCamera;
		std::vector<float> terrain = GenerateTerrain();

		std::vector<BasicVertex> vertices;
		vertices.reserve(myWidth * myHeight);


		for (int y = 0; y < myHeight; ++y)
		{
			for (int x = 0; x < myWidth; ++x)
			{
				BasicVertex vertex;

				// Calculate vertex position
				vertex.myPos.x = static_cast<float>(x);
				vertex.myPos.y = terrain[y * myWidth + x];  // Use terrain height data
				vertex.myPos.z = static_cast<float>(y);

				// Calculate texture coordinates
				vertex.myTexCoord.x = static_cast<float>(x) / (static_cast<float>(myWidth));
				vertex.myTexCoord.y = static_cast<float>(y) / (static_cast<float>(myHeight));

				vertices.push_back(vertex);
			}
		}
		InitTextures();
		std::vector<DWORD> indices;


		for (int y = 0; y < myHeight - 1; ++y)
		{
			for (int x = 0; x < myWidth - 1; ++x)
			{
				// Calculate indices for a quad (two triangles)
				DWORD index1 = y * myWidth + x;
				DWORD index2 = y * myWidth + x + 1;
				DWORD index3 = (y + 1) * myWidth + x;
				DWORD index4 = (y + 1) * myWidth + x + 1;

				// First triangle
				indices.push_back(index1);
				indices.push_back(index3);
				indices.push_back(index2);

				// Second triangle
				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index4);
			}
		}



		VertexFunc::CalculateNormals(vertices, indices);
		VertexFunc::CalculateTangentsAndBinormals(vertices, indices);

		HRESULT hr = myVertexBuffer.Init(aDevice.Get(), vertices.data(), (DWORD)vertices.size());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create vertex buffer.");
			return false;
		}

		hr = myIndexBuffer.Init(aDevice.Get(), indices.data(), (DWORD)indices.size());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create index buffer.");
			return false;
		}

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{"BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		UINT numElements = ARRAYSIZE(layout);

		if (!myVertexShader.Init(aDevice, L"../bin/shaders/TerrainShaderVertex.cso", layout, numElements))
		{
			return false;
		}

		if (!myPixelShader.Init(aDevice, L"../bin/shaders/TerrainShaderPixel.cso"))
		{
			return false;
		}

		hr = myConstantBuffer.Init(aDevice.Get(), aDeviceContext.Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create constant buffer.");
			return false;
		}

		return true;
	}

	void TerrainGenerator::InitTextures()
	{
		std::wstring grassAlbedoPath = L"../bin/assets/textures/terrain/Grass_c.dds";
		std::wstring grassNormalPath = L"../bin/assets/textures/terrain/Grass_n.dds";
		std::wstring grassMetalPath = L"../bin/assets/textures/terrain/Grass_m.dds";

		std::wstring rockAlbedoPath = L"../bin/assets/textures/terrain/Rock_c.dds";
		std::wstring rockNormalPath = L"../bin/assets/textures/terrain/Grass_n.dds";
		std::wstring rockMetalPath = L"../bin/assets/textures/terrain/Rock_m.dds";

		std::wstring snowAlbedoPath = L"../bin/assets/textures/terrain/snow_c.dds";
		std::wstring snowNormalPath = L"../bin/assets/textures/terrain/Grass_n.dds";
		std::wstring snowMetalPath = L"../bin/assets/textures/terrain/snow_m.dds";

		//DirectX::CreateDDSTextureFromFileEx()

		
		HRESULT hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), grassAlbedoPath.c_str(), nullptr, myGrassAlbedoTexture.GetAddressOf(), 0);		
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}
		hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), grassNormalPath.c_str(), nullptr, myGrassNormalTexture.GetAddressOf(), 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}
		hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), grassMetalPath.c_str(), nullptr, myGrassMetalTexture.GetAddressOf(), 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}


		hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), rockAlbedoPath.c_str(), nullptr, myRockAlbedoTexture.GetAddressOf(), 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}

		hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), rockNormalPath.c_str(), nullptr, myRockNormalTexture.GetAddressOf(), 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}
		hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), rockMetalPath.c_str(), nullptr, myRockMetalTexture.GetAddressOf(), 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}


		hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), snowAlbedoPath.c_str(), nullptr, mySnowAlbedoTexture.GetAddressOf(), 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}

		hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), snowNormalPath.c_str(), nullptr, mySnowNormalTexture.GetAddressOf(), 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}
		hr = DirectX::CreateDDSTextureFromFile(Engine::GetInstance()->GetDevice().Get(), snowMetalPath.c_str(), nullptr, mySnowMetalTexture.GetAddressOf(), 0);
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating texture from file.");
		}

		Engine::GetInstance()->GetDeviceContext()->GenerateMips(myGrassAlbedoTexture.Get());
		Engine::GetInstance()->GetDeviceContext()->GenerateMips(myGrassNormalTexture.Get());
		Engine::GetInstance()->GetDeviceContext()->GenerateMips(myGrassMetalTexture.Get());

		Engine::GetInstance()->GetDeviceContext()->GenerateMips(myRockAlbedoTexture.Get());
		Engine::GetInstance()->GetDeviceContext()->GenerateMips(myRockNormalTexture.Get());
		Engine::GetInstance()->GetDeviceContext()->GenerateMips(myRockMetalTexture.Get());

		Engine::GetInstance()->GetDeviceContext()->GenerateMips(mySnowAlbedoTexture.Get());
		Engine::GetInstance()->GetDeviceContext()->GenerateMips(mySnowNormalTexture.Get());
		Engine::GetInstance()->GetDeviceContext()->GenerateMips(mySnowMetalTexture.Get());

	}

	void TerrainGenerator::BindTextures()
	{
		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(4, 1, myGrassAlbedoTexture.GetAddressOf());
		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(5, 1, myGrassNormalTexture.GetAddressOf());
		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(6, 1, myGrassMetalTexture.GetAddressOf());

		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(7, 1, myRockAlbedoTexture.GetAddressOf());
		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(8, 1, myRockNormalTexture.GetAddressOf());
		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(9, 1, myRockMetalTexture.GetAddressOf());

		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(10, 1, mySnowAlbedoTexture.GetAddressOf());
		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(11, 1, mySnowNormalTexture.GetAddressOf());
		Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(12, 1, mySnowMetalTexture.GetAddressOf());
	}

	

	std::vector<float> TerrainGenerator::GenerateTerrain()
	{
		std::vector<float> terrain;
		terrain.reserve(myWidth * myHeight);

		float minHeight = FLT_MAX;
		float maxHeight = FLT_MIN;

		for (int y = 0; y < myHeight; ++y)
		{
			for (int x = 0; x < myWidth; ++x)
			{
				float sampleX = (x - myWidth / 2.0f) / myPerlinScale;
				float sampleY = (y - myHeight / 2.0f) / myPerlinScale;

				float noiseHeight = 0.0f;
				float frequency = 1.0f;
				float amplitude = 1.0f;
				float maxAmplitude = 0.0f;

				for (int octave = 0; octave < myOctaves; ++octave)
				{
					noiseHeight += myPerlinNoise.noise2D(sampleX * frequency, sampleY * frequency) * amplitude;
					frequency *= myLacunarity;
					amplitude *= myPersistence;
					maxAmplitude += amplitude;
				}

				noiseHeight /= maxAmplitude;

				terrain.push_back(noiseHeight);

				if (noiseHeight < minHeight)
					minHeight = noiseHeight;

				if (noiseHeight > maxHeight)
					maxHeight = noiseHeight;
			}
		}

		float heightRange = (maxHeight != minHeight) ? (maxHeight - minHeight) : 1.0f;

		for (float& height : terrain)
		{
			height = ((height - minHeight) / heightRange) * myScale.y;
		}

		return terrain;
	}

	void TerrainGenerator::Render(ID3D11DeviceContext* aDeviceContext)
	{
		UINT offset = 0;
		aDeviceContext->IASetInputLayout(myVertexShader.GetInputLayout());
		aDeviceContext->VSSetShader(myVertexShader.GetShader(), NULL, 0);
		aDeviceContext->PSSetShader(myPixelShader.GetShader(), NULL, 0);

		aDeviceContext->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), myVertexBuffer.GetStridePtr(), &offset);
		aDeviceContext->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		aDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(myPosition.x, myPosition.y, myPosition.z);
		DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(myRotation.x, myRotation.y, myRotation.z);
		DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(1.0f, 1.0f, 1.0f);

		DirectX::XMMATRIX objectMatrix = scaleMatrix * rotationMatrix * translationMatrix;

		myConstantBuffer.myData.ObjectWorldMatrix = objectMatrix;

		BindTextures();

		myConstantBuffer.ApplyChanges();
		aDeviceContext->VSSetConstantBuffers(6, 1, myConstantBuffer.GetAddressOf());
		aDeviceContext->PSSetConstantBuffers(6, 1, myConstantBuffer.GetAddressOf());
		aDeviceContext->PSSetConstantBuffers(1, 1, Engine::GetInstance()->GetLightBufferAddress());

		aDeviceContext->DrawIndexed(myIndexBuffer.GetBufferSize(), 0, 0);
	}
}