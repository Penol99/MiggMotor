#pragma once
#include <d3d11.h>
#include <wrl/client.h>
#include "Shaders.h"
#include "ConstantBuffer.h"
#include "StructuredBuffer.h"

using namespace Microsoft::WRL;

namespace mi
{
    struct SpectrumSettings
    {
        float scale;
        float windSpeed;
        float windDirection;
        float fetch;
        float spreadBlend;
        float swell;
        float peakEnhancement;
        float shortWavesFade;
    };
    struct SpectrumParameters
    {
        float scale;
        float angle;
        float spreadBlend;
        float swell;
        float alpha;
        float peakOmega;
        float gamma;
        float shortWavesFade;
    };
    class UAVRenderTarget;

	class Cascade
	{
	public:
        Cascade();
        ~Cascade();
		void Init(SpectrumSettings someSpectrumSettingsOne, SpectrumSettings someSpectrumSettingsTwo, int aSize, float aLengthScale, float cutOffLow, float cutOffHigh, ID3D11ShaderResourceView* noiseSRV);
		void Update();
        UAVRenderTarget* myDisplacement;
        UAVRenderTarget* myDerivatives;
        UAVRenderTarget* myTurbulence;
        ConstantBuffer<CB_CascadeSettings> myCascadeSettings;

    private:
        float JonswapAlpha(float g, float fetch, float windSpeed);
        float JonswapPeakFrequency(float g, float fetch, float windSpeed);
        void InitShaders();


        int mySize;
        ComputeShader myInitalSpectrumComputeShader;
        ComputeShader myConjugateSpectrumComputeShader;
        ComputeShader mySpectrumEvolutionComputeShader;
        ComputeShader myTwiddleFactorPrecomputationComputeShader;
        ComputeShader myHorizontalInverseFFTComputeShader;
        ComputeShader myVerticalInverseFFTComputeShader;
        ComputeShader myResultScalingComputeShader;
        ComputeShader myFFTOutputPermutationComputeShader;
        ComputeShader myOutputTextureAssignmentComputeShader;
        StructuredBuffer<SpectrumParameters> mySpectrumParameters;

    
        UAVRenderTarget* myInitialSpectrumUAV;
        UAVRenderTarget* myPrecomputedData;
        UAVRenderTarget* myPrecomputedDataFFT;

        UAVRenderTarget* myBuffer;
        UAVRenderTarget* DxDz;
        UAVRenderTarget* DyDxz;
        UAVRenderTarget* DyxDyz;
        UAVRenderTarget* DxxDzz;
        ID3D11ShaderResourceView* myNoiseSRV;

        
    };
}
