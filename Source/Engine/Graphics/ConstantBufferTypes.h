#pragma once
#include <DirectXMath.h>
#include <iostream>
#include "PropertyDefines.h"

#pragma warning(push)
#pragma warning(disable: 4324)
namespace mi
{

	struct CB_ObjectWorldMatrix
	{
		DirectX::XMMATRIX ObjectWorldMatrix;
	};
	
	struct alignas(16)CB_CascadeSettings
	{
		unsigned int Size;
		float LengthScale;
		float CutoffHigh;
		float CutoffLow;

		float GravityAcceleration;
		float Depth;
		float DeltaTime;
		float Time;

		float Lambda;
		bool PingPong;
		unsigned int Step;
		float Garbage;
	};

	struct CB_CommonBuffer {
		DirectX::XMFLOAT4 CameraPosition;
		DirectX::XMMATRIX WorldMatrix;
		DirectX::XMMATRIX ProjectionMatrix;
		DirectX::XMMATRIX ViewMatrix;
	};
	struct CB_CellShaderBuffer {
		DirectX::XMFLOAT2 ScreenSize;
		DirectX::XMFLOAT2 garbageMan;

		float EdgeThreshold;
		DirectX::XMFLOAT3 garbageBoy;
	};

	struct CB_TimeBuffer
	{
		DirectX::XMFLOAT3 color;
		float time;
		DirectX::XMFLOAT2 directionVector;
		float frequency;
		float amplitude;
		float lengthenUV;
		bool isLooping;
		bool ispadd;
		bool ispadd2;
		bool ispadd3;
		float padd1;
		float padd2;
	};

	struct CB_BlurBuffer
	{
		DirectX::XMFLOAT2 RenderResolution;
		bool IsHorizontal;
		bool garbage0;
		bool garbage1;
		bool garbage2;
		float garbage3;
	};

	struct CB_BoneBuffer
	{
		DirectX::XMMATRIX BoneTransforms[MAX_ANIMATION_BONES];
	};
	struct CB_ShadowBuffer
	{
		DirectX::XMMATRIX DirLightViewMatrix;
		DirectX::XMMATRIX DirProjectionMatrix;
		float shadowBias = 0.001f;
	};


	struct CB_PointLightData
	{
		DirectX::XMFLOAT4 Position;
		DirectX::XMFLOAT4 Color;
		float Range;
		DirectX::XMFLOAT3 garbage;
	};

	struct CB_SpotLightData // Byte 64 = 4 * 16
	{
		DirectX::XMFLOAT4 Position; // Byte 16

		DirectX::XMFLOAT4 Color; // Byte 16

		DirectX::XMFLOAT4 Direction; // Byte 16

		float Range; // Byte 4
		float OuterAngle; // Byte 4
		float InnerAngle; // Byte 4
		float garbage; // Byte 4
	};

	struct CB_LightConstantBufferData
	{
		CB_PointLightData myPointLights[NUMBER_OF_LIGHTS_ALLOWED];
		CB_SpotLightData mySpotLights[NUMBER_OF_SPOT_LIGHTS_ALLOWED];

		UINT NumberOfPointLights;
		UINT NumberOfSpotLights;
		int NumEnvMapMipLevels;
		float garbage0;


		DirectX::XMFLOAT4 AmbientLightColor;

		DirectX::XMFLOAT4 Direction;

		DirectX::XMFLOAT3 LightColor;
		float Intensity;


	};


}
#pragma warning(pop)