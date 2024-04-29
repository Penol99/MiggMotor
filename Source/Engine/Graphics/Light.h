#pragma once
#include "Vector.h"
#include "PropertyDefines.h"
#include <vector>
#include <memory>

namespace mi
{

	class Light
	{
	public:
		struct DirectionalLight
		{
		public:
			DirectionalLight() {};
			inline static mi::Vector4f myAmbientColorAndIntensity = mi::Vector4f{ 1.0f, 1.0f, 1.0f, 1.0f };
			inline static mi::Vector4f myDirectionalLightDirection = mi::Vector4f{ 1.0, 1.0f, 1.0f, 1.0f };
			inline static mi::Vector4f myDirectionalLightColor = mi::Vector4f(1.f, 1.f, 1.f);
			inline static float myDirectionalLightIntensity = 1.f;
			static DirectX::XMMATRIX CreateLightWorldMatrix(int aDir);
		};
		struct PointLight
		{
		public:
			PointLight() : myRange(1.f), myPosition({ 0.f,0.f,0.f,0.f }), myColor({ 1.f,1.f,1.f,1.f }) {};
			mi::Vector4f myPosition;
			mi::Vector4f myColor;
			float myRange;
			mi::Vector3f garbage;
		};

		struct SpotLight
		{
		public:
			SpotLight() : myPosition(0.f, 0.f, 0.f, 0.f), myColor(1.f, 1.f, 1.f, 1.f), myRange(1.f),
				myDirection(0.f, 0.f, 0.f, 0.f), myOuterAngle(1.f), myInnerAngle(1.f) {};

			mi::Vector4f myPosition;
			mi::Vector4f myColor;
			mi::Vector4f myDirection;
			float myRange;
			float myOuterAngle;
			float myInnerAngle;
			float garbage=0;

		};

		inline static std::vector<std::shared_ptr<PointLight>> RenderedPointLights;
		inline static std::vector<std::shared_ptr<SpotLight>> RenderedSpotLights;




	};
}


