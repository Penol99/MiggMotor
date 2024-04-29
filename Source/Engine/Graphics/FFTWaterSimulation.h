#pragma once
#include <DirectXMath.h>
#include <vector>
#include "VertexBuffer.h" 
#include "IndexBuffer.h"
#include "Camera.h" 
#include <Graphics/Vertex.h>
#include <Component.h>
#include <Cascade.h>

using Microsoft::WRL::ComPtr;




namespace mi
{


	class FFTWaterSimulation : public  Component
	{
	public:
		FFTWaterSimulation();
		~FFTWaterSimulation();

		
		void Init(const int& aSize);
		void Update(const float aDeltaTime);
		void Render();
		void RenderTool();
		void InitSpectrumParameters();
		bool InitBuffers();


	private:
		bool InitShaders();

		Cascade* cascade0;
		Cascade* cascade1;
		Cascade* cascade2;

		int mySize;
		Camera* myCamera;

		VertexShader myVertexShader;
		PixelShader myPixelShader;
		ComputeShader myGaussianNoiseComputeShader;


		GeometryShader myParticleGeometryShader;
		VertexBuffer<BasicVertex> myVertexBuffer;
		IndexBuffer myIndexBuffer;

		SpectrumSettings mySpectrumSettings[2];
		ConstantBuffer<CB_ObjectWorldMatrix> myObjectWorldMatrix;
		UAVRenderTarget* myNoiseUAV;

	};
}
