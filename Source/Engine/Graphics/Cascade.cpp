#include "engine_pch.h"
#include "Cascade.h"
#include "Engine.h"
#include <RenderTarget.h>



mi::Cascade::Cascade()
{
}

mi::Cascade::~Cascade()
{
}

void mi::Cascade::Init(SpectrumSettings someSpectrumSettingsOne, SpectrumSettings someSpectrumSettingsTwo, int aSize, float aLengthScale, float cutOffLow, float cutOffHigh, ID3D11ShaderResourceView* noiseSRV)
{
	mySize = aSize;
	auto& engine = *Engine::GetInstance();

	myNoiseSRV = noiseSRV;
	HRESULT hr = DirectX::CreateDDSTextureFromFile(engine.GetDevice().Get(), L"Assets/Textures/noiseTexture.dds", nullptr, &myNoiseSRV);
	if (FAILED(hr))
	{
		ErrorLog::Log(hr, "Failed to load noiseTExture texture from file");
	}
	myInitialSpectrumUAV = new UAVRenderTarget();
	myInitialSpectrumUAV->Init(mySize, mySize, DXGI_FORMAT_R32G32B32A32_FLOAT);
	myPrecomputedData = new UAVRenderTarget();
	myPrecomputedData->Init(mySize, mySize, DXGI_FORMAT_R32G32B32A32_FLOAT);
	myDisplacement = new UAVRenderTarget();
	myDisplacement->Init(mySize, mySize, DXGI_FORMAT_R32G32B32A32_FLOAT);
	myDerivatives = new UAVRenderTarget();
	myDerivatives->Init(mySize, mySize, DXGI_FORMAT_R32G32B32A32_FLOAT,true);
	myTurbulence = new UAVRenderTarget();
	myTurbulence->Init(mySize, mySize, DXGI_FORMAT_R32G32B32A32_FLOAT, true);

	myPrecomputedDataFFT = new UAVRenderTarget();
	myPrecomputedDataFFT->Init((int)log2(mySize), mySize, DXGI_FORMAT_R32G32B32A32_FLOAT);
	myBuffer = new UAVRenderTarget();
	myBuffer->Init(mySize, mySize, DXGI_FORMAT_R32G32_FLOAT);
	DxDz = new UAVRenderTarget();
	DxDz->Init(mySize, mySize, DXGI_FORMAT_R32G32_FLOAT);
	DyDxz = new UAVRenderTarget();
	DyDxz->Init(mySize, mySize, DXGI_FORMAT_R32G32_FLOAT);
	DyxDyz = new UAVRenderTarget();
	DyxDyz->Init(mySize, mySize, DXGI_FORMAT_R32G32_FLOAT);
	DxxDzz = new UAVRenderTarget();
	DxxDzz->Init(mySize, mySize, DXGI_FORMAT_R32G32_FLOAT);


	myCascadeSettings.Init(engine.GetDevice().Get(), engine.GetDeviceContext().Get());

	myCascadeSettings.myData.Size = mySize;
	myCascadeSettings.myData.LengthScale = aLengthScale;
	myCascadeSettings.myData.GravityAcceleration = 9.81f;
	myCascadeSettings.myData.Depth = 500.0f;
	myCascadeSettings.myData.Time = 0.f;
	myCascadeSettings.myData.Lambda = 1.0f;
	myCascadeSettings.myData.PingPong = false;
	myCascadeSettings.myData.CutoffLow = cutOffLow;
	myCascadeSettings.myData.CutoffHigh = cutOffHigh;
	myCascadeSettings.ApplyChanges();

	InitShaders();
	SpectrumParameters defaultParams[2];


	defaultParams[0].scale = someSpectrumSettingsOne.scale;
	defaultParams[0].angle = someSpectrumSettingsOne.windDirection / 180 * FMath::Pi;
	defaultParams[0].swell = FMath::Clamp(someSpectrumSettingsOne.swell, 0.01f, 1.f);
	defaultParams[0].alpha = JonswapAlpha(myCascadeSettings.myData.GravityAcceleration, someSpectrumSettingsOne.fetch, someSpectrumSettingsOne.windSpeed);
	defaultParams[0].peakOmega = JonswapPeakFrequency(myCascadeSettings.myData.GravityAcceleration, someSpectrumSettingsOne.fetch, someSpectrumSettingsOne.windSpeed);
	defaultParams[0].gamma = someSpectrumSettingsOne.peakEnhancement;
	defaultParams[0].shortWavesFade = someSpectrumSettingsOne.shortWavesFade;
	defaultParams[0].spreadBlend = someSpectrumSettingsOne.spreadBlend;

	defaultParams[1].scale = someSpectrumSettingsTwo.scale;
	defaultParams[1].angle = someSpectrumSettingsTwo.windDirection / 180 * FMath::Pi;
	defaultParams[1].swell = FMath::Clamp(someSpectrumSettingsTwo.swell, 0.01f, 1.f);
	defaultParams[1].alpha = JonswapAlpha(myCascadeSettings.myData.GravityAcceleration, someSpectrumSettingsTwo.fetch, someSpectrumSettingsTwo.windSpeed);
	defaultParams[1].peakOmega = JonswapPeakFrequency(myCascadeSettings.myData.GravityAcceleration, someSpectrumSettingsTwo.fetch, someSpectrumSettingsTwo.windSpeed);
	defaultParams[1].gamma = someSpectrumSettingsTwo.peakEnhancement;
	defaultParams[1].shortWavesFade = someSpectrumSettingsTwo.shortWavesFade;
	defaultParams[1].spreadBlend = someSpectrumSettingsTwo.spreadBlend;


	mySpectrumParameters.Init(engine.GetDevice().Get(), engine.GetDeviceContext().Get(), 2);
	mySpectrumParameters.Apply(engine.GetDeviceContext().Get(), defaultParams, ARRAYSIZE(defaultParams));


	const int compute_work_group_dim = 8;
	unsigned int dispatchX = (unsigned int)ceil(mySize / float(compute_work_group_dim));
	unsigned int dispatchY = (unsigned int)ceil(mySize / float(compute_work_group_dim));

	ID3D11UnorderedAccessView* nullUAVs[1] = { nullptr };
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };

	//myPrecomputedData->Init(mySize, mySize, DXGI_FORMAT_R32G32B32A32_FLOAT);


	// Initial Spectrum
	engine.GetDeviceContext()->CSSetShader(myInitalSpectrumComputeShader.GetShader(), nullptr, 0);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, myPrecomputedData->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetShaderResources(53, 1, &myNoiseSRV);
	ID3D11ShaderResourceView* params = mySpectrumParameters.CreateShaderResourceView(engine.GetDevice().Get());
	engine.GetDeviceContext()->CSSetShaderResources(54, 1, &params);
	engine.GetDeviceContext()->CSSetConstantBuffers(10, 1, myCascadeSettings.GetAddressOf());
	engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	engine.GetDeviceContext()->CSSetShaderResources(54, 1, nullSRVs);

	// Conjugate Spectrum
	engine.GetDeviceContext()->CSSetShader(myConjugateSpectrumComputeShader.GetShader(), nullptr, 0);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, myInitialSpectrumUAV->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);


	// Dispatch the twiddle shader
	engine.GetDeviceContext()->CSSetShader(myTwiddleFactorPrecomputationComputeShader.GetShader(), nullptr, 0);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, myPrecomputedDataFFT->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
}

void mi::Cascade::InitShaders()
{
	auto& engine = *Engine::GetInstance();


	if (!myInitalSpectrumComputeShader.Init(engine.GetDevice(), L"Shaders/InitalSpectrumCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader InitalSpectrumCompute");
	}
	if (!myConjugateSpectrumComputeShader.Init(engine.GetDevice(), L"Shaders/ConjugateSpectrumCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader ConjugateSpectrumCompute");
	}
	if (!mySpectrumEvolutionComputeShader.Init(engine.GetDevice(), L"Shaders/SpectrumEvolutionCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader SpectrumEvolutionCompute");
	}
	if (!myTwiddleFactorPrecomputationComputeShader.Init(engine.GetDevice(), L"Shaders/TwiddleFactorPrecomputationCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader TwiddleFactorPrecomputationCompute");
	}
	if (!myHorizontalInverseFFTComputeShader.Init(engine.GetDevice(), L"Shaders/HorizontalInverseFFTCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader HorizontalInverseFFTCompute");
	}
	if (!myVerticalInverseFFTComputeShader.Init(engine.GetDevice(), L"Shaders/VerticalInverseFFTCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader VerticalInverseFFTCompute");
	}
	if (!myResultScalingComputeShader.Init(engine.GetDevice(), L"Shaders/ResultScalingCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader ResultScalingCompute");
	}
	if (!myFFTOutputPermutationComputeShader.Init(engine.GetDevice(), L"Shaders/FFTOutputPermutationCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader FFTOutputPermutationCompute");
	}
	if (!myOutputTextureAssignmentComputeShader.Init(engine.GetDevice(), L"Shaders/OutputTextureAssignmentCompute.cso"))
	{
		ErrorLog::Log("Failed Initializing Compute Shader OutputTextureAssignmentCompute");
	}
}

void mi::Cascade::Update()
{




	auto& engine = *Engine::GetInstance();


	myCascadeSettings.myData.DeltaTime = (float)engine.GetTimer().GetDeltaTime();
	myCascadeSettings.myData.Time = (float)engine.GetTimer().GetTotalTime();
	myCascadeSettings.ApplyChanges();




	const int compute_work_group_dim = 8;
	unsigned int dispatchX = (unsigned int)ceil(mySize / float(compute_work_group_dim));
	unsigned int dispatchY = (unsigned int)ceil(mySize / float(compute_work_group_dim));

	ID3D11UnorderedAccessView* nullUAVs[1] = { nullptr };
	ID3D11ShaderResourceView* nullSRVs[1] = { nullptr };

	bool scale = false;
	bool permute = true;

	//// Initial Spectrum
	//engine.GetDeviceContext()->CSSetShader(myInitalSpectrumComputeShader.GetShader(), nullptr, 0);
	//engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, myPrecomputedData->GetUAV().GetAddressOf(), nullptr);
	//engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
	//engine.GetDeviceContext()->CSSetShaderResources(53, 1, &myNoiseSRV);
	//ID3D11ShaderResourceView* params = mySpectrumParameters.CreateShaderResourceView(engine.GetDevice().Get());
	//engine.GetDeviceContext()->CSSetShaderResources(54, 1, &params);
	//engine.GetDeviceContext()->CSSetConstantBuffers(10, 1, myCascadeSettings.GetAddressOf());
	//engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
	//engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	//engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
	//engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	//engine.GetDeviceContext()->CSSetShaderResources(54, 1, nullSRVs);

	//// Conjugate Spectrum
	//engine.GetDeviceContext()->CSSetShader(myConjugateSpectrumComputeShader.GetShader(), nullptr, 0);
	//engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, myInitialSpectrumUAV->GetUAV().GetAddressOf(), nullptr);
	//engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
	//engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
	//engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	//engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);


	// Time-Dependent Spectrum
	engine.GetDeviceContext()->CSSetShader(mySpectrumEvolutionComputeShader.GetShader(), nullptr, 0);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxDz->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, DyDxz->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(2, 1, DyxDyz->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(3, 1, DxxDzz->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetShaderResources(53, 1, myInitialSpectrumUAV->GetShaderResourceView().GetAddressOf());
	engine.GetDeviceContext()->CSSetShaderResources(54, 1, myPrecomputedData->GetShaderResourceView().GetAddressOf());
	engine.GetDeviceContext()->CSSetConstantBuffers(10, 1, myCascadeSettings.GetAddressOf());
	engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(2, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(3, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	engine.GetDeviceContext()->CSSetShaderResources(54, 1, nullSRVs);


	// Inverse FFT (Dx_Dz)

	int logSize = (int)(log2(mySize));

	myCascadeSettings.myData.PingPong = false;
	myCascadeSettings.ApplyChanges();

	for (int i = 0; i < logSize; i++)
	{
		myCascadeSettings.myData.PingPong = !myCascadeSettings.myData.PingPong;
		myCascadeSettings.myData.Step = i;

		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetShader(myHorizontalInverseFFTComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxDz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, myPrecomputedDataFFT->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	}
	myCascadeSettings.myData.PingPong = false;
	myCascadeSettings.ApplyChanges();

	for (int i = 0; i < logSize; i++)
	{
		myCascadeSettings.myData.PingPong = !myCascadeSettings.myData.PingPong;
		myCascadeSettings.myData.Step = i;

		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetShader(myVerticalInverseFFTComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxDz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, myPrecomputedDataFFT->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	}
	if (permute)
	{
		engine.GetDeviceContext()->CSSetShader(myFFTOutputPermutationComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxDz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	}



	// Inverse FFT (Dy_Dxz)
	myCascadeSettings.myData.PingPong = false;
	myCascadeSettings.ApplyChanges();

	for (int i = 0; i < logSize; i++)
	{
		myCascadeSettings.myData.PingPong = !myCascadeSettings.myData.PingPong;
		myCascadeSettings.myData.Step = i;

		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetShader(myHorizontalInverseFFTComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyDxz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, myPrecomputedDataFFT->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	}
	myCascadeSettings.myData.PingPong = false;
	myCascadeSettings.ApplyChanges();

	for (int i = 0; i < logSize; i++)
	{
		myCascadeSettings.myData.PingPong = !myCascadeSettings.myData.PingPong;
		myCascadeSettings.myData.Step = i;

		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetShader(myVerticalInverseFFTComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyDxz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, myPrecomputedDataFFT->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	}
	if (permute) {

		engine.GetDeviceContext()->CSSetShader(myFFTOutputPermutationComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyDxz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	}


	myCascadeSettings.myData.PingPong = false;
	myCascadeSettings.ApplyChanges();
	// Inverse FFT (Dyx_Dyz)

	for (int i = 0; i < logSize; i++)
	{
		myCascadeSettings.myData.PingPong = !myCascadeSettings.myData.PingPong;
		myCascadeSettings.myData.Step = i;

		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetShader(myHorizontalInverseFFTComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyxDyz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, myPrecomputedDataFFT->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	}
	myCascadeSettings.myData.PingPong = false;
	myCascadeSettings.ApplyChanges();

	for (int i = 0; i < logSize; i++)
	{
		myCascadeSettings.myData.PingPong = !myCascadeSettings.myData.PingPong;
		myCascadeSettings.myData.Step = i;

		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetShader(myVerticalInverseFFTComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyxDyz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, myPrecomputedDataFFT->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	}
	if (permute) {
		engine.GetDeviceContext()->CSSetShader(myFFTOutputPermutationComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyxDyz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	}


	myCascadeSettings.myData.PingPong = false;
	myCascadeSettings.ApplyChanges();

	// Inverse FFT (Dxx_Dzz)
	for (int i = 0; i < logSize; i++)
	{
		myCascadeSettings.myData.PingPong = !myCascadeSettings.myData.PingPong;
		myCascadeSettings.myData.Step = i;

		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetShader(myHorizontalInverseFFTComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxxDzz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, myPrecomputedDataFFT->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	}
	myCascadeSettings.myData.PingPong = false;

	for (int i = 0; i < logSize; i++)
	{
		myCascadeSettings.myData.PingPong = !myCascadeSettings.myData.PingPong;
		myCascadeSettings.myData.Step = i;

		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetShader(myVerticalInverseFFTComputeShader.GetShader(), nullptr, 0);
		myCascadeSettings.ApplyChanges();
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxxDzz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myBuffer->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, myPrecomputedDataFFT->GetShaderResourceView().GetAddressOf());
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
		engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	}
	if (permute) {
		engine.GetDeviceContext()->CSSetShader(myFFTOutputPermutationComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxxDzz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	}
	
	
	
	if (scale)
	{
		engine.GetDeviceContext()->CSSetShader(myResultScalingComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxDz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);


		engine.GetDeviceContext()->CSSetShader(myResultScalingComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyDxz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);

		engine.GetDeviceContext()->CSSetShader(myResultScalingComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyxDyz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);

		engine.GetDeviceContext()->CSSetShader(myResultScalingComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DyxDyz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);

		engine.GetDeviceContext()->CSSetShader(myResultScalingComputeShader.GetShader(), nullptr, 0);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, DxxDzz->GetUAV().GetAddressOf(), nullptr);
		engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
		engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	}


	// Output Textures
	engine.GetDeviceContext()->CSSetShader(myOutputTextureAssignmentComputeShader.GetShader(), nullptr, 0);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, myDisplacement->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, myDerivatives->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(2, 1, myTurbulence->GetUAV().GetAddressOf(), nullptr);
	engine.GetDeviceContext()->CSSetShaderResources(53, 1, DxDz->GetShaderResourceView().GetAddressOf());
	engine.GetDeviceContext()->CSSetShaderResources(54, 1, DyDxz->GetShaderResourceView().GetAddressOf());
	engine.GetDeviceContext()->CSSetShaderResources(55, 1, DyxDyz->GetShaderResourceView().GetAddressOf());
	engine.GetDeviceContext()->CSSetShaderResources(56, 1, DxxDzz->GetShaderResourceView().GetAddressOf());
	engine.GetDeviceContext()->CSSetConstantBuffers(10, 1, myCascadeSettings.GetAddressOf());
	engine.GetDeviceContext()->Dispatch(dispatchX, dispatchY, 1);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetUnorderedAccessViews(2, 1, nullUAVs, nullptr);
	engine.GetDeviceContext()->CSSetShaderResources(53, 1, nullSRVs);
	engine.GetDeviceContext()->CSSetShaderResources(54, 1, nullSRVs);
	engine.GetDeviceContext()->CSSetShaderResources(55, 1, nullSRVs);
	engine.GetDeviceContext()->CSSetShaderResources(56, 1, nullSRVs);

	// Generate mipmaps for derivatives and turbulence textures
	engine.GetDeviceContext()->GenerateMips(myDerivatives->GetShaderResourceView().Get());
	engine.GetDeviceContext()->GenerateMips(myTurbulence->GetShaderResourceView().Get());

	ImGui::Begin("Displacement");
	ImGui::Image(myDisplacement->GetShaderResourceView().Get(), ImVec2(256, 256));
	ImGui::End();
	ImGui::Begin("Derivatives");
	ImGui::Image(myDerivatives->GetShaderResourceView().Get(), ImVec2(256, 256));
	ImGui::End();
}

float mi::Cascade::JonswapAlpha(float g, float fetch, float windSpeed)
{
	return 0.076f * pow(g * fetch / windSpeed / windSpeed, -0.22f);
}


float mi::Cascade::JonswapPeakFrequency(float g, float fetch, float windSpeed)
{
	return 22 * pow(windSpeed * fetch / g / g, -0.33f);
}
