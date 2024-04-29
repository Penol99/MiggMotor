#include "engine_pch.h"
#define _XM_NO_INTRINSICS_
#include "ModelComponent.h"
#include "GameObject.h"
#include "Engine.h"

namespace mi
{
	ModelComponent::ModelComponent()
		: Component()
	{
	}
	ModelComponent::~ModelComponent()
	{

	}
	void ModelComponent::Init()
	{
		Engine& engine = *Engine::GetInstance();

		HRESULT hr = myCBObjectMatrix.Init(Engine::GetInstance()->GetDevice().Get(), Engine::GetInstance()->GetDeviceContext().Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating object matrix buffer.");
			return;
		}

		hr = myCBObjectMatrix.Init(engine.GetDevice().Get(), engine.GetDeviceContext().Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating object world matrix buffer.");

		}

		//myVertexShader = Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetVertexShader(0); // Make Shader Manager
		//myPixelShader = Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetPixelShader(0);

	}

	void ModelComponent::Render()
	{
		//int i = 0;

		//if (myVertexShader->GetBuffer() != NULL)
		//{
		//	Engine::GetInstance()->GetDeviceContext()->IASetInputLayout(myVertexShader->GetInputLayout());
		//	Engine::GetInstance()->GetDeviceContext()->VSSetShader(myVertexShader->GetShader(), NULL, 0);
		//}
		//for (Mesh& mesh : myModelInfo->myMeshes)
		//{
		//	if (myPixelShader->GetBuffer() != NULL)
		//	{
		//		Engine::GetInstance()->GetDeviceContext()->PSSetShader(myPixelShader->GetShader(), NULL, 0);
		//	}
		//	MaterialData* materialData = myGameObject->GetMaterials().empty() ? nullptr : myGameObject->GetMaterials()[i];
		//	if (materialData)
		//	{
		//		if (materialData->myAlbedoTexture != NULL)
		//		{
		//			Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, materialData->myAlbedoTexture.GetAddressOf());
		//		}
		//		if (!myVFX)
		//		{
		//			if (materialData->myNormalTexture != NULL)
		//			{
		//				Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(2, 1, materialData->myNormalTexture.GetAddressOf());
		//			}
		//			if (materialData->myMetallicTexture != NULL)
		//			{
		//				Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(3, 1, materialData->myMetallicTexture.GetAddressOf());

		//			}
		//		}
		//	}


		//	DirectX::XMMATRIX scaleMatrix = DirectX::XMMatrixScaling(
		//		myGameObject->myTransform.myScale.x,
		//		myGameObject->myTransform.myScale.y, 
		//		myGameObject->myTransform.myScale.z
		//	);

		//	DirectX::XMMATRIX rotationMatrix = DirectX::XMMatrixRotationRollPitchYaw(
		//		myGameObject->myTransform.myRotation.x,
		//		myGameObject->myTransform.myRotation.y,
		//		myGameObject->myTransform.myRotation.z
		//	);

		//	DirectX::XMMATRIX translationMatrix = DirectX::XMMatrixTranslation(
		//		myGameObject->myTransform.myPosition.x, 
		//		myGameObject->myTransform.myPosition.y,
		//		myGameObject->myTransform.myPosition.z
		//	);

		//	// Apply scaling, rotation and translation in order
		//	
		//	myGameObject->myTransform.myWorldTransform = scaleMatrix * rotationMatrix * translationMatrix;
		//	if (GameObject* parent = myGameObject->GetParent())
		//	{
		//		myGameObject->myTransform.myWorldTransform *= parent->myTransform.myWorldTransform;
		//	}

		//	myCBObjectMatrix.myData.ObjectWorldMatrix = myGameObject->myTransform.myWorldTransform;
		//	myCBObjectMatrix.ApplyChanges();

		//	Engine::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(6, 1, myCBObjectMatrix.GetAddressOf());
		//	Engine::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(1, 1, Engine::GetInstance()->GetLightBufferAddress());

		//	mesh.Render(Engine::GetInstance()->GetDeviceContext().Get());
		//	if (myGameObject->GetMaterials().size() < i)
		//	{

		//		i++;
		//	}
		//}
	}

	void ModelComponent::SetPixelShader(int aId)
	{
		aId;
		//myPixelShader = Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetPixelShader(aId);
	}

	void ModelComponent::SetVertexShader(int aId)
	{
		aId;
		//myVertexShader = Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetVertexShader(aId);
	}

	void ModelComponent::SetModelInfo(std::string aPath)
	{
		aPath;
		//myModelInfo = &Engine::GetInstance()->GetResourceBank()->GetModelBank()->GetModelInfo(aPath);
	}

	void ModelComponent::AddMaterial(std::string aPath)
	{
		aPath;
		//myGameObject->GetMaterials().push_back(Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetMaterial(aPath));
	}

	void ModelComponent::AddMaterial(int aId)
	{
		aId;
		//myGameObject->GetMaterials().push_back(Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetMaterial(aId));
	}

}