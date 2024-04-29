#include "engine_pch.h"
#include "Light.h"

namespace mi
{


	DirectX::XMMATRIX Light::DirectionalLight::CreateLightWorldMatrix(int aDir)
	{

		DirectX::XMVECTOR lightDirection = DirectX::XMVectorSet(myDirectionalLightDirection.x, myDirectionalLightDirection.y, myDirectionalLightDirection.z, 0.0f);

		const float distance = 1.f * (float)aDir;
		DirectX::XMVECTOR lightPosition = DirectX::XMVectorScale(lightDirection, distance);

		DirectX::XMVECTOR upDirection = DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		return DirectX::XMMatrixLookAtLH(lightPosition, DirectX::XMVectorZero(), upDirection);
	}
}