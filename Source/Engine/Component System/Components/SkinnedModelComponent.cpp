#include "engine_pch.h"
#define _XM_NO_INTRINSICS_
#include "SkinnedModelComponent.h"
#include "GameObject.h"
#include "Engine.h"

namespace mi
{
	SkinnedModelComponent::SkinnedModelComponent()
		: Component()
	{

	}
	SkinnedModelComponent::~SkinnedModelComponent()
	{

	}
	void SkinnedModelComponent::Init()
	{
		Engine& engine = *Engine::GetInstance();

		std::vector<DirectX::XMMATRIX> empty(MAX_ANIMATION_BONES, DirectX::XMMatrixIdentity());
		memcpy(myBoneTransforms, empty.data(), MAX_ANIMATION_BONES * sizeof(DirectX::XMMatrixIdentity()));

		HRESULT hr = myCBObjectMatrix.Init(Engine::GetInstance()->GetDevice().Get(), Engine::GetInstance()->GetDeviceContext().Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating object matrix buffer.");
			return;
		}
		hr = myBoneBuffer.Init(Engine::GetInstance()->GetDevice().Get(), Engine::GetInstance()->GetDeviceContext().Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating bone buffer.");
			return;
		}

		hr = myCBObjectMatrix.Init(engine.GetDevice().Get(), engine.GetDeviceContext().Get());
		if (FAILED(hr))
		{
			ErrorLog::Log(hr, "failed creating object world matrix buffer.");

		}

		//myVertexShader = Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetVertexShader(2);
		//myPixelShader = Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetPixelShader(0);

	}

	void SkinnedModelComponent::Render()
	{

		
		if (myVertexShader->GetBuffer() != NULL)
		{
			Engine::GetInstance()->GetDeviceContext()->IASetInputLayout(myVertexShader->GetInputLayout());
			Engine::GetInstance()->GetDeviceContext()->VSSetShader(myVertexShader->GetShader(), NULL, 0);
		}
		for (Mesh& mesh : myModelInfo->myMeshes)
		{
			mesh;
			if (myPixelShader->GetBuffer() != NULL)
			{
				Engine::GetInstance()->GetDeviceContext()->PSSetShader(myPixelShader->GetShader(), NULL, 0);
			}
			/*MaterialData* materialData = myGameObject->GetMaterials().empty() ? nullptr : myGameObject->GetMaterials()[i];
			if (materialData)
			{
				if (myMaterialDatas[i]->myAlbedoTexture != NULL)
				{
					Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(1, 1, myMaterialDatas[i]->myAlbedoTexture.GetAddressOf());
				}
				if (myMaterialDatas[i]->myNormalTexture != NULL)
				{
					Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(2, 1, myMaterialDatas[i]->myNormalTexture.GetAddressOf());
				}
				if (myMaterialDatas[i]->myMetallicTexture != NULL)
				{
					Engine::GetInstance()->GetDeviceContext()->PSSetShaderResources(3, 1, myMaterialDatas[i]->myMetallicTexture.GetAddressOf());
				}
			}*/

			

			//myCBObjectMatrix.myData.ObjectWorldMatrix = myGameObject->myTransform.myWorldTransform;
			//memcpy(myBoneBuffer.myData.BoneTransforms, myBoneTransforms, MAX_ANIMATION_BONES * sizeof(DirectX::XMMATRIX));



			//myBoneBuffer.ApplyChanges();
			//myCBObjectMatrix.ApplyChanges();
			//Engine::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(3, 1, myBoneBuffer.GetAddressOf());
			//Engine::GetInstance()->GetDeviceContext()->VSSetConstantBuffers(6, 1, myCBObjectMatrix.GetAddressOf());
			//Engine::GetInstance()->GetDeviceContext()->PSSetConstantBuffers(1, 1, Engine::GetInstance()->GetLightBufferAddress());


			//mesh.Render(Engine::GetInstance()->GetDeviceContext().Get());
			//if (myGameObject->GetMaterials().size() < i)
			//{
			//	i++;
			//}
		}
	}

	void SkinnedModelComponent::SetPixelShader(int aId)
	{
		aId;
		//myPixelShader = Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetPixelShader(aId);
	}

	void SkinnedModelComponent::SetVertexShader(int aId)
	{
		aId;
		//myVertexShader = Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetVertexShader(aId);
	}

	void SkinnedModelComponent::SetModelInfo(std::string aPath)
	{
		aPath;
		//myModelInfo = &Engine::GetInstance()->GetResourceBank()->GetModelBank()->GetSkinnedModelInfo(aPath);
	}

	void SkinnedModelComponent::AddMaterial(std::string aPath)
	{
		aPath;
		//myMaterialDatas.push_back(Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetMaterial(aPath));
	}

	void SkinnedModelComponent::AddMaterial(int aId)
	{
		aId;
		//myMaterialDatas.push_back(Engine::GetInstance()->GetResourceBank()->GetMaterialBank()->GetMaterial(aId));
	}

	void SkinnedModelComponent::ClearMaterials()
	{
		myMaterialDatas.clear();
	}

}