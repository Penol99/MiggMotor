#include <engine_pch.h>
#include "FFTWaterSimulation.h"
#include <Engine.h>
#include <GameObject.h>
#include <RenderTarget.h>
#define MAX_PARTICLES 131072/2
namespace mi
{
	FFTWaterSimulation::FFTWaterSimulation()
	{

	}
	FFTWaterSimulation::~FFTWaterSimulation()
	{
		delete cascade0;
		delete cascade1;
		delete cascade2;
		delete myNoiseUAV;
	}


	void FFTWaterSimulation::Init(const int& aSize)
	{
		auto& engine = *Engine::GetInstance();

		mySize = aSize;
		if (!InitBuffers())
		{
			ErrorLog::Log("Failed Initializing FFTWaterSimulation Buffers");
		}
		if (!InitShaders())
		{
			ErrorLog::Log("Failed Initializing FFTWaterSimulation Shaders");
		}
		myNoiseUAV = new UAVRenderTarget();
		myNoiseUAV->Init(mySize, mySize, DXGI_FORMAT_R32G32_FLOAT);

		const int compute_work_group_dim = 8;
		unsigned int dispatchX = (unsigned int)ceil(mySize / float(compute_work_group_dim));
		unsigned int dispatchY = (unsigned int)ceil(mySize / float(compute_work_group_dim));
		ID3D11UnorderedAccessView* nullUAVs[1] = { nullptr };

		// Generate Gaussian Noise Texture
		engine.GetDeviceContext()->CSSetShader(myGaussianNoiseComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, myNoiseUAV->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		
		myCamera = &engine.GetCamera();
		InitSpectrumParameters();

		cascade0 = new Cascade();
		cascade1 = new Cascade();
		cascade2 = new Cascade();

		const float lengthScale0 = 250.f;
		const float lengthScale1 = 17.f;
		const float lengthScale2 = 5.f;
		const float boundary1 = 2 * FMath::Pi / lengthScale1 * 6.f;
		const float boundary2 = 2 * FMath::Pi / lengthScale2 * 6.f;
		cascade0->Init(mySpectrumSettings[0], mySpectrumSettings[1], mySize, lengthScale0, 0.001f,boundary1, myNoiseUAV->GetShaderResourceView().Get());
		cascade1->Init(mySpectrumSettings[0], mySpectrumSettings[1], mySize, lengthScale1, boundary1, boundary2, myNoiseUAV->GetShaderResourceView().Get());
		cascade2->Init(mySpectrumSettings[0], mySpectrumSettings[1], mySize, lengthScale2, boundary2,9999.f, myNoiseUAV->GetShaderResourceView().Get());

	}

	void FFTWaterSimulation::Update(const float aDeltaTime)
	{
		auto& engine = *Engine::GetInstance();

		UNREFERENCED_PARAMETER(engine);
		UNREFERENCED_PARAMETER(aDeltaTime);
	}

	void FFTWaterSimulation::Render()
	{
		cascade0->Update();
		cascade1->Update();
		cascade2->Update();
		
		
		auto& engine = *Engine::GetInstance();

		UINT offset = 0;
		myObjectWorldMatrix.myData.ObjectWorldMatrix = myGameObject->myTransform.GetWorldTransform();
		myObjectWorldMatrix.ApplyChanges();

		engine.GetDeviceContext()->IASetInputLayout(myVertexShader.GetInputLayout());
		engine.GetDeviceContext()->VSSetShader(myVertexShader.GetShader(), NULL, 0);
		engine.GetDeviceContext()->PSSetShader(myPixelShader.GetShader(), NULL, 0);

		engine.GetDeviceContext()->IASetVertexBuffers(0, 1, myVertexBuffer.GetAddressOf(), myVertexBuffer.GetStridePtr(), &offset);
		engine.GetDeviceContext()->IASetIndexBuffer(myIndexBuffer.Get(), DXGI_FORMAT_R32_UINT, 0);
		engine.GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


		engine.GetDeviceContext()->VSSetConstantBuffers(10, 1, cascade0->myCascadeSettings.GetAddressOf());
		engine.GetDeviceContext()->VSSetConstantBuffers(6, 1, myObjectWorldMatrix.GetAddressOf());

		engine.GetDeviceContext()->VSSetShaderResources(54, 1, cascade0->myDisplacement->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->VSSetShaderResources(55, 1, cascade1->myDisplacement->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->VSSetShaderResources(56, 1, cascade2->myDisplacement->GetShaderResourceView().GetAddressOf());

		engine.GetDeviceContext()->PSSetShaderResources(15, 1, cascade0->myDerivatives->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->PSSetShaderResources(16, 1, cascade1->myDerivatives->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->PSSetShaderResources(17, 1, cascade2->myDerivatives->GetShaderResourceView().GetAddressOf());

		engine.GetDeviceContext()->PSSetShaderResources(18, 1, cascade0->myTurbulence->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->PSSetShaderResources(19, 1, cascade1->myTurbulence->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->PSSetShaderResources(20, 1, cascade2->myTurbulence->GetShaderResourceView().GetAddressOf());


		engine.GetDeviceContext()->DrawIndexed(myIndexBuffer.GetBufferSize(), 0, 0);

		ID3D11ShaderResourceView* nullSRV[] = { nullptr };
		engine.GetDeviceContext()->VSSetShaderResources(54, 1, nullSRV);
		engine.GetDeviceContext()->VSSetShaderResources(55, 1, nullSRV);
		engine.GetDeviceContext()->VSSetShaderResources(56, 1, nullSRV);
		engine.GetDeviceContext()->PSSetShaderResources(15, 1, nullSRV);
		engine.GetDeviceContext()->PSSetShaderResources(16, 1, nullSRV);
		engine.GetDeviceContext()->PSSetShaderResources(17, 1, nullSRV);
		engine.GetDeviceContext()->PSSetShaderResources(18, 1, nullSRV);
		engine.GetDeviceContext()->PSSetShaderResources(19, 1, nullSRV);
		engine.GetDeviceContext()->PSSetShaderResources(20, 1, nullSRV);
	
	}

	void FFTWaterSimulation::RenderTool()
	{


	}

	void FFTWaterSimulation::InitSpectrumParameters()
	{
		//mySpectrumSettings[0].scale = 1;
		//mySpectrumSettings[0].windSpeed = 0.5;
		//mySpectrumSettings[0].windDirection = -29.81f;
		//mySpectrumSettings[0].fetch = 100000;
		//mySpectrumSettings[0].spreadBlend = 1;
		//mySpectrumSettings[0].swell = 0.198f;
		//mySpectrumSettings[0].peakEnhancement = 3.3f;
		//mySpectrumSettings[0].shortWavesFade = 0.01f;

		//mySpectrumSettings[1].scale = 0;
		//mySpectrumSettings[1].windSpeed = 1;
		//mySpectrumSettings[1].windDirection = 0;
		//mySpectrumSettings[1].fetch = 300000;
		//mySpectrumSettings[1].spreadBlend = 1;
		//mySpectrumSettings[1].swell = 1;
		//mySpectrumSettings[1].peakEnhancement = 3.3f;
		//mySpectrumSettings[1].shortWavesFade = 0.01f;

		mySpectrumSettings[0].scale = 2.5f;  
		mySpectrumSettings[0].windSpeed = 1.5f;  
		mySpectrumSettings[0].windDirection = -29.81f;
		mySpectrumSettings[0].fetch = 200000;  
		mySpectrumSettings[0].spreadBlend = 0.8f;  
		mySpectrumSettings[0].swell = 0.3f;  
		mySpectrumSettings[0].peakEnhancement = 4.5f;  
		mySpectrumSettings[0].shortWavesFade = 0.005f;  

		mySpectrumSettings[1].scale = 1.5f;  
		mySpectrumSettings[1].windSpeed = 2.0f;  
		mySpectrumSettings[1].windDirection = 0;
		mySpectrumSettings[1].fetch = 400000;  
		mySpectrumSettings[1].spreadBlend = 0.9f;  
		mySpectrumSettings[1].swell = 1.2f;  
		mySpectrumSettings[1].peakEnhancement = 4.0f; 
		mySpectrumSettings[1].shortWavesFade = 0.008f;  
		
		//mySpectrumSettings[0].scale = 0.5f;
		//mySpectrumSettings[0].windSpeed = 0.1f;
		//mySpectrumSettings[0].windDirection = -29.81f;
		//mySpectrumSettings[0].fetch = 50000;
		//mySpectrumSettings[0].spreadBlend = 0.8f;
		//mySpectrumSettings[0].swell = 0.1f;
		//mySpectrumSettings[0].peakEnhancement = 1.5f;
		//mySpectrumSettings[0].shortWavesFade = 0.05f;

		//mySpectrumSettings[1].scale = 0;
		//mySpectrumSettings[1].windSpeed = 0.1f;
		//mySpectrumSettings[1].windDirection = 0;
		//mySpectrumSettings[1].fetch = 150000;
		//mySpectrumSettings[1].spreadBlend = 0.8f;
		//mySpectrumSettings[1].swell = 0.5f;
		//mySpectrumSettings[1].peakEnhancement = 1.5f;
		//mySpectrumSettings[1].shortWavesFade = 0.05f;
	}

	bool FFTWaterSimulation::InitBuffers()
	{
		auto& engine = *Engine::GetInstance();
		myObjectWorldMatrix.Init(engine.GetDevice().Get(), engine.GetDeviceContext().Get());
		std::vector<BasicVertex> vertices;
		vertices.reserve((size_t)(mySize * mySize));

		float vertexSpacingScalar = 1.f;
		for (int y = 0; y < mySize; ++y)
		{
			for (int x = 0; x < mySize; ++x)
			{
				BasicVertex vertex;

				// Calculate vertex position
				vertex.myPos.x = (static_cast<float>(x) - mySize / 2) * vertexSpacingScalar;
				vertex.myPos.y = 1.f;
				vertex.myPos.z = (static_cast<float>(y) - mySize / 2) * vertexSpacingScalar;

				// Calculate texture coordinates
				vertex.myTexCoord.x = static_cast<float>(x) / (static_cast<float>(mySize) - 1);
				vertex.myTexCoord.y = 1.0f - static_cast<float>(y) / (static_cast<float>(mySize) - 1);

				vertices.push_back(vertex);
			}
		}
		std::vector<DWORD> indices;


		for (int y = 0; y < mySize - 1; ++y)
		{
			for (int x = 0; x < mySize - 1; ++x)
			{

				DWORD index1 = y * (DWORD)mySize + x;
				DWORD index2 = y * (DWORD)mySize + x + 1;
				DWORD index3 = (y + 1) * (DWORD)mySize + x;
				DWORD index4 = (y + 1) * (DWORD)mySize + x + 1;

				indices.push_back(index1);
				indices.push_back(index3);
				indices.push_back(index2);

				indices.push_back(index2);
				indices.push_back(index3);
				indices.push_back(index4);
			}
		}

		VertexFunc::CalculateNormals(vertices, indices);
		VertexFunc::CalculateTangentsAndBinormals(vertices, indices);

		HRESULT hr = myVertexBuffer.Init(engine.GetDevice().Get(), vertices.data(), (DWORD)vertices.size());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create vertex buffer.");
			return false;
		}

		hr = myIndexBuffer.Init(engine.GetDevice().Get(), indices.data(), (DWORD)indices.size());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "Failed to create index buffer.");
			return false;
		}
		return true;
	}


	bool FFTWaterSimulation::InitShaders()
	{
		auto& engine = *Engine::GetInstance();

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BINORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		UINT numElements = ARRAYSIZE(layout);
		if (!myGaussianNoiseComputeShader.Init(engine.GetDevice(), L"Shaders/GaussianNoiseTextureCompute.cso"))
		{
			ErrorLog::Log("Failed Initializing Compute Shader GaussianNoiseTextureCompute");
		}
		if (!myVertexShader.Init(engine.GetDevice(), L"Shaders/WaterShaderVertex.cso", layout, numElements))
		{
			return false;
		}
		if (!myPixelShader.Init(engine.GetDevice(), L"Shaders/WaterShaderPixel.cso"))
		{
			return false;
		}

		return true;
	}



}
